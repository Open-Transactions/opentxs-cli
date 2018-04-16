/************************************************************
 *
 *                 OPEN TRANSACTIONS
 *
 *       Financial Cryptography and Digital Cash
 *       Library, Protocol, API, Server, CLI, GUI
 *
 *       -- Anonymous Numbered Accounts.
 *       -- Untraceable Digital Cash.
 *       -- Triple-Signed Receipts.
 *       -- Cheques, Vouchers, Transfers, Inboxes.
 *       -- Basket Currencies, Markets, Payment Plans.
 *       -- Signed, XML, Ricardian-style Contracts.
 *       -- Scripted smart contracts.
 *
 *  EMAIL:
 *  fellowtraveler@opentransactions.org
 *
 *  WEBSITE:
 *  http://www.opentransactions.org/
 *
 *  -----------------------------------------------------
 *
 *   LICENSE:
 *   This Source Code Form is subject to the terms of the
 *   Mozilla Public License, v. 2.0. If a copy of the MPL
 *   was not distributed with this file, You can obtain one
 *   at http://mozilla.org/MPL/2.0/.
 *
 *   DISCLAIMER:
 *   This program is distributed in the hope that it will
 *   be useful, but WITHOUT ANY WARRANTY; without even the
 *   implied warranty of MERCHANTABILITY or FITNESS FOR A
 *   PARTICULAR PURPOSE.  See the Mozilla Public License
 *   for more details.
 *
 ************************************************************/

#include "CmdCancel.hpp"

#include "CmdBase.hpp"
#include "CmdDeposit.hpp"

#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include "opentxs/api/client/ServerAction.hpp"
#include <opentxs/client/ServerAction.hpp>
#include <opentxs/client/SwigWrap.hpp>
#include "opentxs/core/recurring/OTPaymentPlan.hpp"
#include "opentxs/core/script/OTSmartContract.hpp"
#include <opentxs/core/Identifier.hpp>
#include <opentxs/core/Log.hpp>
#include <opentxs/OT.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdCancel::CmdCancel()
{
    command = "cancel";
    args[0] = "--mynym <nym>";
    args[1] = "[--myacct <account>]";
    args[2] = "--indices <indices|all>";
    category = catInstruments;
    help = "Cancel an uncashed outgoing instrument from outpayment box.";
    usage = "Specify --myacct when canceling a smart contract.";
}

CmdCancel::~CmdCancel() {}

int32_t CmdCancel::runWithOptions()
{
    return run(getOption("mynym"), getOption("myacct"), getOption("indices"));
}

// Also TODO:
// Update the "cancel" command, for outgoing cash, to give you a choice to
// deposit the cash instead
// of discarding it.
// CmdDeposit::depositPurse(strNotaryID, strMyAcctID, strToNymID, strInstrument,
// "") // strIndices is left blank in this case

// NOTE: You can't just discard a sent cheque  from your outpayment box.
// Why not? Just because you remove your record of the outgoing cheque,
// doesn't mean you didn't already send it. (The recipient still received
// it, and still has it, whether you remove it from your outbox or not.)
// If you really want to cancel the cheque, then you need to do it in such
// a way that it will fail-as-cancelled when the recipient tries to deposit
// it. Otherwise, he would get that money whether you kept your own outgoing
// record or not. Therefore SOME server message must be performed here,
// which actually cancels the transaction number itself that appears on the
// cheque. This is the only way to insure that the cheque can't be used by
// the recipient (and even this will only work if you beat him to the punch.
// If he deposits it before you cancel it, then it's already too late and he
// has the money.) THIS is why RecordPayment, regarding outpayments, only
// works on expired instruments -- because if it's not expired, you don't
// just want to remove your record of it. You want to cancel the transaction
// number itself -- and that requires server communication.

int32_t CmdCancel::run(string mynym, string myacct, string indices)
{
    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    if ("" != myacct && !checkAccount("myacct", myacct)) {
        return -1;
    }

    if (!checkIndices("indices", indices)) {
        return -1;
    }

    int32_t items = SwigWrap::GetNym_OutpaymentsCount(mynym);
    if (0 > items) {
        otOut << "Error: cannot load payment outbox item count.\n";
        return -1;
    }

    if (0 == items) {
        otOut << "The payment outbox is empty.\n";
        return 0;
    }

    bool all = "all" == indices;

    // Loop from back to front, in case any are removed.
    int32_t retVal = 1;
    for (int32_t i = items - 1; 0 <= i; i--) {
        if (!all && !SwigWrap::NumList_VerifyQuery(indices, to_string(i))) {
            continue;
        }

        string payment = SwigWrap::GetNym_OutpaymentsContentsByIndex(mynym, i);
        if ("" == payment) {
            otOut << "Error: cannot load payment " << i << ".\n";
            retVal = -1;
            continue;
        }

        string server = SwigWrap::GetNym_OutpaymentsNotaryIDByIndex(mynym, i);
        if ("" == server) {
            otOut << "Error: cannot load server for payment " << i << ".\n";
            retVal = -1;
            continue;
        }

        // Done: Put the code here where we message the server to cancel all
        // relevant transaction numbers for the instrument. If it's a cheque,
        // there's only one number. But if it's a smart contract, there could
        // be many numbers. Seems like best thing is to just activate it, but
        // have a "rejected" flag which causes the activation to fail. (That
        // way, all the other parties will be properly notified, which the
        // server already does.) We don't even need to remove it from the
        // outpayment box, because the failure notification from the server
        // will already cause the OTClient to remove it from the outpayment box.
        //
        // Ah-ha! ANY outgoing payment plan or smart contract is necessarily
        // incomplete: it's outgoing because it was sent to the next party so
        // he could sign it, too, and probably activate it. Presumably he has
        // not done so yet (if I am 'beating him to the punch' by cancelling it
        // before he can activate it) and therefore the plan or smart contract
        // still is missing at least one signer, so it is GUARANTEED to fail
        // verification if I try to activate it myself. (Good.)
        //
        // This means I can just take whatever instrument appears outgoing,
        // and try to activate it myself. It will definitely fail activation,
        // and then the failure notice will already be sent from that, to all
        // the parties, and they can harvest back their numbers automatically
        // as necessary.
        //
        // The one problem is, though this works for payment plans and smart
        // contracts, it will not work for cheques. The cheque is made out to
        // someone else, and he is the one normally who needs to deposit it.
        // Plus, I can't deposit a cheque into the same account it's drawn on.
        //
        // UPDATE: Now when a cheque is deposited into the same account it was
        // drawn on, that will be interpreted by the server as a request to
        // CANCEL the cheque.

        const Identifier theNotaryID{server}, theNymID{mynym};
        string type = SwigWrap::Instrmnt_GetType(payment);

        if ("SMARTCONTRACT" == type) {
            // Just take the smart contract from the outpayment box, and try to
            // activate it. It WILL fail, and then the failure message will be
            // propagated to all the other parties to the contract. (Which will
            // result in its automatic removal from the outpayment box.)

            // FIX: take myacct from smart contract instead of --myacct
            if ("" == myacct) {
                otOut << "You MUST provide --myacct for smart contracts.\n";
                retVal = -1;
                continue;
            }

            // Try to activate the smart contract. (As a way of  cancelling it.)
            // So while we expect this 'activation' to fail, it should have the
            // desired effect of cancelling the smart contract and sending
            // failure notices to all the parties.

            std::unique_ptr<OTSmartContract> contract =
                std::make_unique<OTSmartContract>();

            OT_ASSERT(contract)

            contract->LoadContractFromString(String(payment));
            string response = OT::App()
                                  .API()
                                  .ServerAction()
                                  .ActivateSmartContract(
                                      theNymID,
                                      theNotaryID,
                                      Identifier(myacct),
                                      "acct_agent_name",
                                      contract)
                                  ->Run();
            if ("" == response) {
                otOut << "Error: cannot cancel smart contract.\n";
                retVal = -1;
                continue;
            }

            otOut << "Server reply: \n" << response << "\n";

            if (1 != SwigWrap::Message_IsTransactionCanceled(
                         server, mynym, myacct, response)) {
                otOut << "Error: cancel smart contract failed.\n";
                retVal = -1;
                continue;
            }

            otOut << "Success canceling smart contract!\n";
            continue;
        }

        if ("PAYMENT PLAN" == type) {
            // Just take the payment plan from the outpayment box, and try to
            // activate it. It WILL fail, and then the failure message will be
            // propagated to the other party to the contract. (Which will result
            // in its automatic removal from the outpayment box.)

            std::unique_ptr<OTPaymentPlan> plan =
                std::make_unique<OTPaymentPlan>();

            OT_ASSERT(plan)

            plan->LoadContractFromString(String(payment));
            string response =
                OT::App()
                    .API()
                    .ServerAction()
                    .CancelPaymentPlan(theNymID, theNotaryID, plan)
                    ->Run();
            if ("" == response) {
                otOut << "Error: cannot cancel payment plan.\n";
                retVal = -1;
                continue;
            }

            otOut << "Server reply: \n" << response << "\n";

            if (1 != SwigWrap::Message_IsTransactionCanceled(
                         server, mynym, myacct, response)) {
                otOut << "Error: cancel payment plan failed.\n";
                retVal = -1;
                continue;
            }

            otOut << "Success canceling payment plan!\n";
            continue;
        }

        if ("PURSE" == type) {
            // This is a tricky one -- why would anyone EVER want to discard
            // outgoing cash? Normally your incentive would be to do the
            // opposite: Keep a copy of all outgoing cash until the copy
            // itself expires (when the cash expires.) This way it's always
            // recoverable in the event of a "worst case" situation.
            //
            // So what do we do in this case? Nevertheless, the user has
            // explicitly just instructed the client to DISCARD OUTGOING CASH.
            //
            // Perhaps we should just ask the user to CONFIRM that he wants to
            // erase the cash, and make SURE that he understands the
            // consequences of that choice.

            // removes payment instrument (from payments in or out box)
            if (!SwigWrap::RecordPayment(server, mynym, false, i, false)) {
                otOut << "Error: cannot cancel cash purse.\n";
                retVal = -1;
                continue;
            }

            otOut << "Success canceling cash purse!\n";
            continue;
        }

        // CHEQUE VOUCHER INVOICE

        bool isVoucher = ("VOUCHER" == type);

        // Get the nym and account IDs from the cheque itself.
        string acctID = isVoucher
                            ? SwigWrap::Instrmnt_GetRemitterAcctID(payment)
                            : SwigWrap::Instrmnt_GetSenderAcctID(payment);
        if ("" == acctID) {
            otOut << "Error: cannot retrieve asset account ID.\n";
            retVal = -1;
            continue;
        }

        string nymID = isVoucher ? SwigWrap::Instrmnt_GetRemitterNymID(payment)
                                 : SwigWrap::Instrmnt_GetSenderNymID(payment);
        if ("" == nymID) {
            otOut << "Error: cannot retrieve sender nym.\n";
            retVal = -1;
            continue;
        }

        if (nymID != mynym) {
            otOut << "Error: unexpected sender nym.\n";
            retVal = -1;
            continue;
        }

        CmdDeposit deposit;
        if (1 != deposit.depositCheque(server, acctID, nymID, payment)) {
            otOut << "Error: cannot cancel " << type << ".\n";
            retVal = -1;
            continue;
        }

        otOut << "Success canceling " << type << "!\n";
    }
    return retVal;
}
