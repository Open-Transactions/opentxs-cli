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

#include "CmdBaseAccept.hpp"

#include "CmdConfirm.hpp"
#include "CmdPayInvoice.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/api/Api.hpp>
#include <opentxs/api/OT.hpp>
#include <opentxs/client/OTAPI_Wrap.hpp>
#include <opentxs/client/OT_ME.hpp>
#include <opentxs/client/MadeEasy.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

/*
call OTAPI_Wrap::LoadInbox() to load the inbox ledger from local storage.

During this time, your user has the opportunity to peruse the inbox, and to
decide which transactions therein he wishes to accept or reject. Usually the
inbox is displayed on the screen, then the user selects various items to accept
or reject, and then the user clicks Process Inbox and then you do this:
Then call OTAPI_Wrap::Ledger_CreateResponse() in order to create a response
ledger for that inbox, which will be sent to the server to signal your responses
to the various inbox transactions.
Then call OTAPI_Wrap::Ledger_GetCount() (pass it the inbox) to find out how many
transactions are inside of it. Use that count to LOOP through them
Use OTAPI_Wrap::Ledger_GetTransactionByIndex() to grab each transaction as you
iterate through the inbox. (There are various introspection functions you can
use in the API here if you wish to display the inbox items on the screen for the
user.)
Next call OTAPI_Wrap::Transaction_CreateResponse() for each transaction in the
inbox, to create a response to it, accepting or rejecting it. This function
creates the response and adds it to the response ledger.
Next, call OTAPI_Wrap::Ledger_FinalizeResponse() which will create a Balance
Agreement for the ledger.
Finally, call OTAPI_Wrap::processInbox() to send your message to the server and
process the various items.

If the message was successful, then use
OTAPI_Wrap::Message_GetBalanceAgreementSuccess() and
OTAPI_Wrap::Message_GetTransactionSuccess() as described above in the deposit
cash instructions.
*/

// Done:  add options here for accept transfers, accept receipts, and accept
// all.
// Done:  Then basically make a version for the payments inbox for accept
// payments, accept invoices, and accept all.
//
// (Accepting payments can basically be automated, but accepting invoices
// requires user permission.)
//
// Therefore add:
//   acceptmoney    -- This accepts all incoming transfers and incoming payments
//                     (Not receipts or invoices) for any designated accounts
// and nyms.
//   acceptreceipts -- Accepts all inbox receipts (not transfers.)
//   acceptinvoices -- Accepts all invoices (not payments.)
//   acceptall      -- All of the above.
//
// Todo: Make the above functions also work with specific indices (vs "all")
//

//
// PROCESS INBOX, ACCEPTING ALL ITEMS WITHIN...
//
// Load an asset account's inbox from local storage and iterate through
// the items inside, and fire off a server message accepting them all.
//
// itemType == 0 for all, 1 for transfers only, 2 for receipts only.
// "" == indices for "all indices"
//
int32_t CmdBaseAccept::acceptFromInbox(const string& myacct,
                                       const string& indices,
                                       const int32_t itemType) const
{
    string server = OTAPI_Wrap::GetAccountWallet_NotaryID(myacct);
    if ("" == server) {
        otOut << "Error: cannot determine server from myacct.\n";
        return -1;
    }

    string mynym = OTAPI_Wrap::GetAccountWallet_NymID(myacct);
    if ("" == mynym) {
        otOut << "Error: cannot determine mynym from myacct.\n";
        return -1;
    }

    if (!OT::App().API().ME().retrieve_account(server, mynym, myacct, true)) {
        otOut << "Error retrieving intermediary files for account.\n";
        return -1;
    }

    // NOTE: Normally we don't have to do this, because the high-level API is
    // smart enough, when sending server transaction requests, to grab new
    // transaction numbers if it is running low. But in this case, we need the
    // numbers available BEFORE sending the transaction request, because the
    // call to OTAPI_Wrap::Ledger_CreateResponse is where the number is first
    // needed, and that call is made before the server transaction request is
    // actually sent.

    if (!OT_ME::It().make_sure_enough_trans_nums(10, server, mynym)) {
        otOut << "Error: cannot reserve transaction numbers.\n";
        return -1;
    }

    string inbox = OTAPI_Wrap::LoadInbox(server, mynym, myacct);
    if ("" == inbox) {
        otOut << "Error: cannot load inbox.\n";
        return -1;
    }

    int32_t items = OTAPI_Wrap::Ledger_GetCount(server, mynym, myacct, inbox);
    if (0 > items) {
        otOut << "Error: cannot load inbox item count.\n";
        return -1;
    }

    if (0 == items) {
        otOut << "The inbox is empty.\n";
        return 0;
    }

    if (!checkIndicesRange("indices", indices, items)) {
        return -1;
    }

    bool all = "" == indices || "all" == indices;

    string ledger = "";
    for (int32_t i = 0; i < items; i++) {
        string tx = OTAPI_Wrap::Ledger_GetTransactionByIndex(server, mynym,
                                                             myacct, inbox, i);

        // itemType == 0 for all, 1 for transfers only, 2 for receipts only.
        if (0 != itemType) {
            string type =
                OTAPI_Wrap::Transaction_GetType(server, mynym, myacct, tx);
            bool transfer = "pending" == type;
            if (1 == itemType && !transfer) {
                // not a transfer.
                continue;
            }
            if (2 == itemType && transfer) {
                // not a receipt.
                continue;
            }
        }

        // do we want this index?
        if (!all && !OTAPI_Wrap::NumList_VerifyQuery(indices, to_string(i))) {
            continue;
        }

        // need to create response ledger?
        if ("" == ledger) {
            ledger =
                OTAPI_Wrap::Ledger_CreateResponse(server, mynym, myacct, inbox);
            if ("" == ledger) {
                otOut << "Error: cannot create response ledger.\n";
                return -1;
            }
        }

        ledger = OTAPI_Wrap::Transaction_CreateResponse(server, mynym, myacct,
                                                        ledger, tx, true);
        if ("" == ledger) {
            otOut << "Error: cannot create transaction response.\n";
            return -1;
        }
    }

    if ("" == ledger) {
        // did not process anything
        return 0;
    }

    string response =
        OTAPI_Wrap::Ledger_FinalizeResponse(server, mynym, myacct, ledger);
    if ("" == response) {
        otOut << "Error: cannot finalize response.\n";
        return -1;
    }

    response = OT::App().API().ME().process_inbox(server, mynym, myacct, response);
    int32_t reply =
        responseReply(response, server, mynym, myacct, "process_inbox");
    if (1 != reply) {
        return reply;
    }

    if (!OT::App().API().ME().retrieve_account(server, mynym, myacct, true)) {
        otOut << "Error retrieving intermediary files for account.\n";
        return -1;
    }

    return 1;
}

int32_t CmdBaseAccept::acceptFromPaymentbox(const string& myacct,
                                            const string& indices,
                                            const string& paymentType,
                                            string * pOptionalOutput/*=nullptr*/) const
{
    if ("" == myacct) {
        otOut << "Error: myacct is empty.\n";
        return -1;
    }

    string server = OTAPI_Wrap::GetAccountWallet_NotaryID(myacct);
    if ("" == server) {
        otOut << "Error: cannot determine server from myacct.\n";
        return -1;
    }

    string mynym = OTAPI_Wrap::GetAccountWallet_NymID(myacct);
    if ("" == mynym) {
        otOut << "Error: cannot determine mynym from myacct.\n";
        return -1;
    }

    string inbox = OTAPI_Wrap::LoadPaymentInbox(server, mynym);
    if ("" == inbox) {
        otOut << "Error: cannot load payment inbox.\n";
        return -1;
    }

    int32_t items = OTAPI_Wrap::Ledger_GetCount(server, mynym, mynym, inbox);
    if (0 > items) {
        otOut << "Error: cannot load payment inbox item count.\n";
        return -1;
    }

    if (!checkIndicesRange("indices", indices, items)) {
        return -1;
    }

    if (0 == items) {
        otOut << "The payment inbox is empty.\n";
        return 0;
    }

    // Regarding bIsDefinitelyPaymentPlan:
    // I say "definitely" because there are cases where this could be false
    // and it's still a payment plan. For example, someone may have passed
    // "ANY" instead of "PAYMENT PLAN" -- in that case, it's still a payment
    // plan, but at this spot we just don't DEFINITELY know that yet.
    // Is that a problem? No, because processPayment can actually handle that
    // case as well. But I still prefer to handle it higher up (here) where
    // possible, so I can phase out the other. IOW, I actually want to disallow
    // processPayment from processing payment plans. You shouldn't be able
    // to agree to a long-term recurring payment plan by just accepting "all".
    // Rather, you should have to specifically look at that plan, and explicitly
    // confirm your agreement to it, before it can get activated. That's what
    // I'm enforcing here.
    //
    const bool bIsDefinitelyPaymentPlan   = ("PAYMENT PLAN"  == paymentType);
    const bool bIsDefinitelySmartContract = ("SMARTCONTRACT" == paymentType);

    if (bIsDefinitelySmartContract)
    {
        otOut << "acceptFromPaymentbox: It's a bug that this function was even called at all! "
        "You CANNOT confirm smart contracts via this function. "
        "The reason is because you have to select various accounts during the "
        "confirmation process. The function confirmSmartContract would ask various questions "
        "at the command line about which accounts to choose. Thus, you MUST have "
        "your own code in the GUI itself that performs that process for smart contracts.\n";
        return -1;
    }
    // ----------
    bool all = "" == indices || "all" == indices;

    const int32_t nNumlistCount = all ? 0 : OTAPI_Wrap::NumList_Count(indices);

    // NOTE: If we are processing multiple indices, then the return value
    // is 1, since some indices may succeed and some may fail. So our return
    // value merely communicates: The processing was performed.
    //
    // ===> Whereas if there is only ONE index, then we need to set the return
    // value directly to the result of processing that index. Just watch nReturnValue
    // to see how that is being done.
    //
    int32_t nReturnValue = 1;

    for (int32_t i = items - 1; 0 <= i; i--) {
        if (all || OTAPI_Wrap::NumList_VerifyQuery(indices, to_string(i)))
        {
            if (bIsDefinitelyPaymentPlan)
            {

                string instrument = OT_ME::It().get_payment_instrument(server, mynym, i, inbox);
                if ("" == instrument) {
                    otOut << "CmdBaseAccept::acceptFromPaymentbox: "
                        "Error: cannot get payment instrument from inpayments box.\n";
                    return -1;
                }

                CmdConfirm cmd;
                string recipient = OTAPI_Wrap::Instrmnt_GetRecipientNymID(instrument);
                int32_t nTemp = cmd.confirmInstrument(server, mynym, myacct,
                                                      recipient, instrument,
                                                      i, pOptionalOutput);
                if (1 == nNumlistCount) { // If there's exactly 1 instrument being singled-out
                    nReturnValue = nTemp; // for processing, then return its success/fail status.
                    break; // Since there's only one, might as well break;
                }
            }
            else
            {
                CmdPayInvoice payInvoice;
                int32_t nTemp = payInvoice.processPayment(myacct, paymentType,
                                                          inbox, i, pOptionalOutput);
                if (1 == nNumlistCount) { // If there's exactly 1 instrument being singled-out
                    nReturnValue = nTemp; // for processing, then return its success/fail status.
                    break; // Since there's only one, might as well break;
                }
            }
        }
    }

    return nReturnValue;
}
