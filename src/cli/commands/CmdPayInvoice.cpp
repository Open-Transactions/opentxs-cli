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

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdPayInvoice::CmdPayInvoice()
{
    command = "payinvoice";
    args[0] = "--myacct <account>";
    args[1] = "[--server <server>]";
    args[2] = "[--index <index>]";
    category = catOtherUsers;
    help = "Pay an invoice.";
    usage = "If --index is omitted you must paste an invoice. Also, 'server' "
    "should be the 'transport notary' aka where myacct's Nym receives "
    "messages/payments. Default Server will be myacct's Notary ID. This whole "
    "command and CLI tool needs a revamp to take transport notaries into "
    "account. Coming soon.";
}

CmdPayInvoice::~CmdPayInvoice() {}

int32_t CmdPayInvoice::runWithOptions()
{
    return run(getOption("server"), getOption("myacct"), getOption("index"));
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

int32_t CmdPayInvoice::run(string server, string myacct, string index)
{
    if (!checkAccount("myacct", myacct)) {
        return -1;
    }

    if (!checkServer("server", server)) {
        server = SwigWrap::GetAccountWallet_NotaryID(myacct);
    }
    if (!checkServer("server", server)) {
        return -1;
    }

    if ("" != index && !checkValue("index", index)) {
        return -1;
    }

    return processPayment(
        server, myacct, "INVOICE", "", "" == index ? -1 : stoi(index));
}

int32_t CmdPayInvoice::processPayment(
    const string& transport_notary,
    const string& myacct,
    const string& paymentType,
    const string& inbox,
    const int32_t index,
    string* pOptionalOutput /*=nullptr*/)
{
    return OTRecordList::processPayment(
        transport_notary,
        myacct,
        paymentType,
        inbox,
        index,
        pOptionalOutput,
        true);  // CLI_input_allowed = true.
}
