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

#include "CmdPayInvoice.hpp"

#include "CmdBase.hpp"
#include "CmdConfirm.hpp"
#include "CmdDeposit.hpp"

#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/OT.hpp>
#include <opentxs/client/OT_ME.hpp>
#include <opentxs/client/SwigWrap.hpp>
#include <opentxs/core/util/Common.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdPayInvoice::CmdPayInvoice()
{
    command = "payinvoice";
    args[0] = "--myacct <account>";
    args[1] = "[--index <index>]";
    category = catOtherUsers;
    help = "Pay an invoice.";
    usage = "If --index is omitted you must paste an invoice.";
}

CmdPayInvoice::~CmdPayInvoice()
{
}

int32_t CmdPayInvoice::runWithOptions()
{
    return run(getOption("myacct"), getOption("index"));
}

// Should I bother moving the invoice from the payments inbox to the record box?
//
// Update: Okay for now, I'm using an API call here (RecordPayment) which moves
// the invoice. HOWEVER, in the real term, we don't want to do it here. Rather,
// we want to do it inside OT while it's processesing the server reply for your
// cheque (invoice) deposit.
// For example what if there's a network problem and we don't process that reply
// here now? There'll still be a copy of the reply in the Nymbox and it will
// still get processed at a future time... and THAT's when we need to move the
// record, not here. (But this is what we'll do for now.)

// UPDATE:
// - In my Payments Inbox, there could be a cheque or invoice. Either way, when
// I deposit the cheque or pay the invoice, the chequeReceipt goes back to the
// signer's asset account's inbox.
// - When he accepts the chequeReceipt (during a processInbox) and WHEN HE GETS
// THE "SUCCESS" REPLY to that processInbox, is when the chequeReceipt should
// be moved from his inbox to his record box. It MUST be done then, inside OT,
// because the next time he downloads the inbox from the server, that
// chequeReceipt won't be in there anymore! It'll be too late to pass it on to
// the records.
// - Whereas I, being the recipient of his cheque, had it in my **payments
// inbox,** and thus upon receipt of a successful server-reply to my deposit
// transaction, need to move it from my payments inbox to my record box. (The
// record box will eventually be a callback so that client software can take
// over that functionality, which is outside the scope of OT. The actual CALL
// to store in the record box, however should occur inside OT.)
// - For now, I'm using the below API call, so it's available inside the
// scripts. This is "good enough" for now, just to get the payments inbox/outbox
// working for the scripts. But in the long term, I'll need to add the hooks
// directly into OT as described just above. (It'll be necessary in order to
// get the record box working.)
// - Since I'm only worried about Payments Inbox for now, and since I'll be
// calling the below function directly from inside the scripts, how will this
// work? Incoming cheque or invoice will be in the payments inbox, and will
// need to be moved to recordBox (below call) when the script receives a
// success reply to the depositing/paying of that cheque/invoice.
// - Whereas outoing cheque/invoice is in the Outpayments box, (fundamentally
// more similar to the outmail box than to the payments inbox.) If the
// cheque/invoice is cashed/paid by the endorsee, **I** will receive the
// chequeReceipt, in MY asset account inbox, and when I accept it during a
// processInbox transaction, the SUCCESS REPLY from the server for that
// processInbox is where I should actually process that chequeReceipt and,
// if it appears in the outpayments box, move it at that time to the record
// box. The problem is, I can NOT do this much inside the script. To do this
// part, I thus HAVE to go into OT itself as I just described.
// - Fuck!
// - Therefore I might as well comment this out, since this simply isn't going
// to work.

// - Updated plan:
// 1. DONE: Inside OT, when processing successful server reply to processInbox
// request, if a chequeReceipt was processed out successfully, and if that
// chequeReceipt is found inside the outpayments, then move it at that time to
// the record box.
// 2. DONE: Inside OT, when processing successful server reply to depositCheque
// request, if that cheque is found inside the Payments Inbox, move it to the
// record box.
// 3. As for cash: If I SENT cash, it will be in my outpayments box. But that's
// wrong. Because I can never see if the guy cashed it or not. Therefore it
// should go straight to the record box, when sent. AND it needs to be
// encrypted to MY key, not his -- so need to generate BOTH versions, when
// exporting the purse to him in the FIRST PLACE. Then my version goes straight
// into my record box and I can delete it at my leisure. (If he comes running
// the next day saying "I lost it!!" I can still recover it. But once he
// deposits it, then the cash will be no good and I might as well archive it
// or destroy it, or whatever I choose to do with my personal records.)
// If I RECEIVED cash, it will be in my payments inbox, and then when I deposit
// it, and when I process the SUCCESSFUL server REPLY to my depositCash request,
// it should be moved to my record Box.
// 4. How about vouchers? If I deposit a voucher, then the "original sender"
// should get some sort of notice. This means attaching his ID to the voucher
// --which should be optional-- and then dropping an "FYI" notice to him when
// it gets deposited. It can't be a normal chequeReceipt because that's used to
// verify the balance agreement against a balance change, whereas a "voucher
// receipt" wouldn't represent a balance change at all, since the balance was
// already changed when you originally bought the voucher.
// Instead it would probably be send to your Nymbox but it COULD NOT BE PROVEN
// that it was, since OT currently can't prove NOTICE!!
//
// All of the above needs to happen inside OT, since there are many places
// where it's the only appropriate place to take the necessary action.

int32_t CmdPayInvoice::run(string myacct, string index)
{
    if (!checkAccount("myacct", myacct)) {
        return -1;
    }

    if ("" != index && !checkValue("index", index)) {
        return -1;
    }

    return processPayment(myacct, "INVOICE", "",
                          "" == index ? -1 : stoi(index));
}

int32_t CmdPayInvoice::processPayment(const string& myacct,
                                      const string& paymentType,
                                      const string& inbox, const int32_t index,
                                      string * pOptionalOutput/*=nullptr*/)
{
    if ("" == myacct) {
        otOut << "Failure: myacct not a valid string.\n";
        return -1;
    }

    string server = SwigWrap::GetAccountWallet_NotaryID(myacct);
    if ("" == server) {
        otOut << "Error: cannot determine server from myacct.\n";
        return -1;
    }

    string mynym = SwigWrap::GetAccountWallet_NymID(myacct);
    if ("" == mynym) {
        otOut << "Error: cannot determine mynym from myacct.\n";
        return -1;
    }

    string instrument = "";
    if (-1 == index) {
        instrument = inputText("the instrument");
        if ("" == instrument) {
            return -1;
        }
    }
    else {

        instrument = OT::App().API().OTME().get_payment_instrument(server, mynym, index, inbox);
        if ("" == instrument) {
            otOut << "Error: cannot get payment instrument.\n";
            return -1;
        }
    }

    string type = SwigWrap::Instrmnt_GetType(instrument);
    if ("" == type) {
        otOut << "Error: cannot determine instrument type.\n";
        return -1;
    }

    string strIndexErrorMsg = "";
    if (-1 != index) {
        strIndexErrorMsg = "at index " + to_string(index) + " ";
    }

    if ("" != paymentType && // If there is a payment type specified..
        paymentType != "ANY" && // ...and if that type isn't "ANY"...
        paymentType != type)    // ...and it's the wrong type:
    {   // Then skip this one.
        // Except:
        if (("CHEQUE" == paymentType && "VOUCHER" == type) ||
            ("VOUCHER" == paymentType && "CHEQUE" == type)) {
            // in this case we allow it to drop through.
        }
        else {
            otOut << "Error: invalid instrument type.\n";
            return -1;
        }
    }

    const bool bIsPaymentPlan   = ("PAYMENT PLAN"  == type);
    const bool bIsSmartContract = ("SMARTCONTRACT" == type);

    if (bIsPaymentPlan) {
        otOut << "Error: Cannot process a payment plan here. You HAVE to explicitly confirm it using confirmInstrument instead of processPayment.\n";
        // NOTE: I could remove this block and it would still work. I'm just
        // deliberately disallowing payment plans here, so you are forced to
        // explicitly confirm a payment plan. Otherwise here you might confirm
        // a dozen plans under "ANY" and it's just too easy for them to slip
        // by.
        return -1;
    }

    if (bIsSmartContract) {
        otOut << "Error: Cannot process a smart contract here. You HAVE to provide that functionality in your GUI directly, since you may have to choose various accounts as part of the activation process, and your user will need to probably do that in a GUI wizard. It's not so simple as in this function where you just have 'myacct'.\n";
        return -1;
    }

    // Note: I USED to check the ASSET TYPE ID here, but then I removed it,
    // since details_deposit_cheque() already verifies that (so I don't need
    // to do it twice.)

    // By this point, we know the invoice has the right instrument definition
    // for the account we're trying to use (to pay it from.)
    //
    // But we need to make sure the invoice is made out to mynym (or to no
    // one.) Because if it IS endorsed to a Nym, and mynym is NOT that nym,
    // then the transaction will fail. So let's check, before we bother
    // sending it...
    string sender    = SwigWrap::Instrmnt_GetSenderNymID(instrument);
    string recipient = SwigWrap::Instrmnt_GetRecipientNymID(instrument);

    string endorsee = bIsPaymentPlan ? sender : recipient;

    // Not all instruments have a specified recipient. But if they do, let's
    // make sure the Nym matches.
    if ("" != endorsee && (endorsee != mynym)) {
        otOut << "The instrument " << strIndexErrorMsg
        << "is endorsed to a specific " << (bIsPaymentPlan ? "customer" : "recipient")
              << " (" << endorsee
              << ") and it doesn't match the account's owner NymId (" << mynym
              << "). This is a problem, for example, because you can't deposit a cheque into your own account, if the cheque is made out to someone else. (Skipping.)\nTry specifying a different "
                 "account, using --myacct ACCT_ID \n";
        return -1;
    }

    // At this point I know the invoice isn't made out to anyone, or if it is,
    // it's properly made out to the owner of the account which I'm trying to
    // use to pay the invoice from. So let's pay it!
    // P.S. recipient might be empty, but mynym is guaranteed to be good.

    string assetType =
        SwigWrap::Instrmnt_GetInstrumentDefinitionID(instrument);
    string accountAssetType = getAccountAssetType(myacct);

    if ("" != assetType && accountAssetType != assetType) {
        otOut << "The instrument at index " << index
              << " has a different instrument definition than the selected "
                 "account. "
                 "(Skipping.)\nTry specifying a different account, using "
                 "--myacct ACCT_ID \n";
        return -1;
    }
    // ---------------------------------------------
    if (bIsPaymentPlan)
    {
        // Note: this block is currently unreachable/disallowed.
        //       (But it would otherwise work.)
        //
        // NOTE: We couldn't even do this for smart contracts, since
        // the "confirmSmartContract" function assumes it's being used
        // at the command line, and it asks the user to enter various
        // data (choose your account, etc) at the command line.
        // So ONLY with Payment Plans can we do this here! The GUI has
        // to provide its own custom code for smart contracts. However,
        // that code will be easy to write: Just copy the code you see
        // in confirmInstrument, for smart contracts, and change it to
        // use GUI input/output instead of command line i/o.
        //
        CmdConfirm cmd;
        return cmd.confirmInstrument(server, mynym, myacct, recipient, instrument,
                                     index, pOptionalOutput);
        // NOTE: we don't perform any RecordPayment here because
        // confirmInstrument already does that.
    }
    // ---------------------------------------------
    time64_t from  = SwigWrap::Instrmnt_GetValidFrom(instrument);
    time64_t until = SwigWrap::Instrmnt_GetValidTo(instrument);
    time64_t now   = SwigWrap::GetTime();

    if (now < from) {
        otOut << "The instrument at index " << index
              << " is not yet within its valid date range. (Skipping.)\n";
        return -1;
    }

    if (until > OT_TIME_ZERO && now > until) {
        otOut << "The instrument at index " << index
              << " is expired. (Moving it to the record box.)\n";

        // Since this instrument is expired, remove it from the payments inbox,
        // and move to record box.
        if (0 <= index &&
            SwigWrap::RecordPayment(server, mynym, true, index, true)) {
            return 0;
        }

        return -1;
    }

    // IMPORTANT: After the below deposits are completed successfully, the
    // wallet will receive a "successful deposit" server reply. When that
    // happens, OT (internally) needs to go and see if the deposited item was a
    // payment in the payments inbox. If so, it should REMOVE it from that box
    // and move it to the record box.
    //
    // That's why you don't see me messing with the payments inbox even when
    // these are successful. They DO need to be removed from the payments inbox,
    // but just not here in the script. (Rather, internally by OT itself.)
    if ("CHEQUE" == type || "VOUCHER" == type || "INVOICE" == type) {
        CmdDeposit deposit;
        return deposit.depositCheque(server, myacct, mynym, instrument, pOptionalOutput);
    }
    else if ("PURSE" == type) {
        CmdDeposit deposit;
        int32_t success =
            deposit.depositPurse(server, myacct, mynym, instrument, "", pOptionalOutput);

        // if index != -1, go ahead and call RecordPayment on the purse at that
        // index, to remove it from payments inbox and move it to the recordbox.
        if (index != -1 && 1 == success) {
            SwigWrap::RecordPayment(server, mynym, true, index, true);
        }

        return success;
    }

    otOut << "\nSkipping this instrument: Expected CHEQUE, VOUCHER, INVOICE, "
             "or (cash) PURSE.\n";

    return -1;
}
