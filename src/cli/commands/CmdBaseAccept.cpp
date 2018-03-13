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

#include <opentxs/api/client/ServerAction.hpp>
#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/client/OT_API.hpp>
#include <opentxs/client/ServerAction.hpp>
#include <opentxs/client/SwigWrap.hpp>
#include <opentxs/core/Identifier.hpp>
#include <opentxs/core/Ledger.hpp>
#include <opentxs/core/Log.hpp>
#include <opentxs/OT.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

/*
call SwigWrap::LoadInbox() to load the inbox ledger from local storage.

During this time, your user has the opportunity to peruse the inbox, and to
decide which transactions therein he wishes to accept or reject. Usually the
inbox is displayed on the screen, then the user selects various items to accept
or reject, and then the user clicks Process Inbox and then you do this:
Then call SwigWrap::Ledger_CreateResponse() in order to create a response
ledger for that inbox, which will be sent to the server to signal your responses
to the various inbox transactions.
Then call SwigWrap::Ledger_GetCount() (pass it the inbox) to find out how many
transactions are inside of it. Use that count to LOOP through them
Use SwigWrap::Ledger_GetTransactionByIndex() to grab each transaction as you
iterate through the inbox. (There are various introspection functions you can
use in the API here if you wish to display the inbox items on the screen for the
user.)
Next call SwigWrap::Transaction_CreateResponse() for each transaction in the
inbox, to create a response to it, accepting or rejecting it. This function
creates the response and adds it to the response ledger.
Next, call SwigWrap::Ledger_FinalizeResponse() which will create a Balance
Agreement for the ledger.
Finally, call SwigWrap::processInbox() to send your message to the server and
process the various items.

If the message was successful, then use
SwigWrap::Message_GetBalanceAgreementSuccess() and
SwigWrap::Message_GetTransactionSuccess() as described above in the deposit
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
int32_t CmdBaseAccept::acceptFromInbox(
    const string& myacct,
    const string& indices,
    const int32_t itemTypeFilter) const
{
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
    // -----------------------------------------------------------
    // NOTE: I just removed this during my most recent changes. It just seems,
    // now with Justus' regular automated refreshes, I shouldn't have to grab
    // these right BEFORE, when I then anyway have to grab them right AFTER
    // (at the end of this function) once it succeeds. Should speed things
    // up?
    // Also, I considered the fact that we're using indices in this function
    // So if the user has actually selected certain indices already, then seems
    // unwise to download the inbox before processing THOSE indices. Rather
    // have it fail and re-try, and at least be trying the actual intended
    // indices, and cut our account retrievals in half while we're at it!
    //
    //    if (!OT::App().API().OTME().retrieve_account(server, mynym, myacct,
    //    true)) {
    //        otOut << "Error retrieving intermediary files for account.\n";
    //        return -1;
    //    }
    // -----------------------------------------------------------
    // NOTE: Normally we don't have to do this, because the high-level API is
    // smart enough, when sending server transaction requests, to grab new
    // transaction numbers if it is running low. But in this case, we need the
    // numbers available BEFORE sending the transaction request, because the
    // call to SwigWrap::Ledger_CreateResponse is where the number is first
    // needed, and that call is made before the server transaction request is
    // actually sent.
    //
    if (!OT::App().API().ServerAction().GetTransactionNumbers(
            Identifier(mynym), Identifier(server), 10)) {
        otOut << "Error: cannot reserve transaction numbers.\n";
        return -1;
    }
    // -----------------------------------------------------------
    const Identifier theNotaryID{server}, theNymID{mynym}, theAcctID{myacct};

    std::unique_ptr<Ledger> pInbox(
        OT::App().API().OTAPI().LoadInbox(theNotaryID, theNymID, theAcctID));
    if (false == bool(pInbox)) {
        otOut << "Error: cannot load inbox.\n";
        return -1;
    }
    // -----------------------------------------------------------
    int32_t item_count = pInbox->GetTransactionCount();
    // -----------------------------------------------------------
    if (0 > item_count) {
        otErr << "Error: cannot load inbox item count.\n";
        return -1;
    } else if (0 == item_count) {
        otWarn << "The inbox is empty.\n";
        return 0;
    }
    // --------------------
    if (!checkIndicesRange("indices", indices, item_count)) {
        return -1;
    }
    bool all = "" == indices || "all" == indices;
    // -----------------------------------------------------------
    std::set<int32_t>* pOnlyForIndices{nullptr};
    std::set<int32_t> setForIndices;
    if (!all) {
        NumList numlistForIndices{indices};
        std::set<int64_t> setForIndices64;
        if (numlistForIndices.Output(setForIndices64)) {
            pOnlyForIndices = &setForIndices;
            for (const int64_t& lIndex : setForIndices64) {
                setForIndices.insert(static_cast<int32_t>(lIndex));
            }
        }
    }
    std::set<int64_t> receiptIds{pInbox->GetTransactionNums(pOnlyForIndices)};

    if (receiptIds.size() < 1) {
        otWarn << "There are no inbox receipts to process.\n";
        return 0;
    }
    // -----------------------------------------------------------
    // NOTE: Indices are only optional. Otherwise it's "accept all receipts".
    // But even if you DID pass in a comma-separated list of indices, it doesn't
    // matter below this point.
    // That's because we have now translated the user-selected GUI indices into
    // an actual set of receipt IDs, each being the trans num on a transaction
    // inside the inbox.
    // -------------------------------------------------------
    OT_API::ProcessInbox response{OT::App().API().OTAPI().Ledger_CreateResponse(
        theNotaryID, theNymID, theAcctID)};
    // -------------------------------------------------------
    auto& processInbox = std::get<0>(response);
    auto& inbox = std::get<1>(response);

    if (!bool(processInbox) || !bool(inbox)) {
        otWarn << __FUNCTION__ << "Ledger_CreateResponse somehow failed.\n";
        return -1;
    }
    // -------------------------------------------------------
    for (const int64_t& lReceiptId : receiptIds) {
        OTTransaction* pReceipt =
            OT::App().API().OTAPI().Ledger_GetTransactionByID(
                *inbox, lReceiptId);

        if (nullptr == pReceipt) {
            otErr << __FUNCTION__
                  << "Unexpectedly got a nullptr for ReceiptId: " << lReceiptId;
            return -1;
        }  // Below this point, pReceipt is a good pointer. It's
        //   owned by inbox, so no need to delete.
        // ------------------------
        // itemTypeFilter == 0 for all, 1 for transfers only, 2 for receipts
        // only.
        if (0 != itemTypeFilter) {
            const OTTransaction::transactionType receipt_type{
                pReceipt->GetType()};
            const bool transfer = (OTTransaction::pending == receipt_type);
            if ((1 == itemTypeFilter) && !transfer) {
                // not a pending transfer.
                continue;
            }
            if ((2 == itemTypeFilter) && transfer) {
                // not a receipt.
                continue;
            }
        }
        // ------------------------
        const bool bReceiptResponseCreated =
            OT::App().API().OTAPI().Transaction_CreateResponse(
                theNotaryID,
                theNymID,
                theAcctID,
                *processInbox,
                *pReceipt,
                true);

        if (!bReceiptResponseCreated) {
            otErr << __FUNCTION__
                  << "Error: cannot create transaction response.\n";
            return -1;
        }
    }  // for
    // -------------------------------------------------------
    if (processInbox->GetTransactionCount() <= 0) {
        // did not process anything
        otErr << __FUNCTION__
              << "Should never happen. Might want to follow up "
                 "if you see this log.\n";
        return 0;
    }
    // ----------------------------------------------
    const bool bFinalized = OT::App().API().OTAPI().Ledger_FinalizeResponse(
        theNotaryID, theNymID, theAcctID, *processInbox);
    if (!bFinalized) {
        otErr << __FUNCTION__ << "Error: cannot finalize response.\n";
        return -1;
    }
    // ----------------------------------------------
    const opentxs::String strFinalized{*processInbox};
    const std::string str_finalized{strFinalized.Get()};
    // ----------------------------------------------
    const std::string notary_response =
        OT::App()
            .API()
            .ServerAction()
            .ProcessInbox(theNymID, theNotaryID, theAcctID, processInbox)
            ->Run();
    int32_t reply =
        responseReply(notary_response, server, mynym, myacct, "process_inbox");

    if (1 != reply) {
        return reply;
    }

    // We KNOW they all just changed, since we just processed
    // the inbox. Might as well refresh our copy with the new changes.
    //
    if (!OT::App().API().ServerAction().DownloadAccount(
            theNymID, theNotaryID, theAcctID, true)) {
        otOut << __FUNCTION__
              << "Success processing inbox, but then failed "
                 "retrieving intermediary files for account.\n";
        //      return -1; // By this point we DID successfully process the
        //      inbox.
        // (We just then subsequently failed to download the updated acct
        // files.)
    }
    return 1;
}

int32_t CmdBaseAccept::acceptFromPaymentbox(
    const string& myacct,
    const string& indices,
    const string& paymentType,
    string* pOptionalOutput /*=nullptr*/) const
{
    if ("" == myacct) {
        otOut << "Error: myacct is empty.\n";
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

    string inbox = SwigWrap::LoadPaymentInbox(server, mynym);
    if ("" == inbox) {
        otOut << "Error: cannot load payment inbox.\n";
        return -1;
    }

    int32_t items = SwigWrap::Ledger_GetCount(server, mynym, mynym, inbox);
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
    const bool bIsDefinitelyPaymentPlan = ("PAYMENT PLAN" == paymentType);
    const bool bIsDefinitelySmartContract = ("SMARTCONTRACT" == paymentType);

    if (bIsDefinitelySmartContract) {
        otOut << "acceptFromPaymentbox: It's a bug that this function was even "
                 "called at all! "
                 "You CANNOT confirm smart contracts via this function. "
                 "The reason is because you have to select various accounts "
                 "during the "
                 "confirmation process. The function confirmSmartContract "
                 "would ask various questions "
                 "at the command line about which accounts to choose. Thus, "
                 "you MUST have "
                 "your own code in the GUI itself that performs that process "
                 "for smart contracts.\n";
        return -1;
    }
    // ----------
    bool all = "" == indices || "all" == indices;

    const int32_t nNumlistCount = all ? 0 : SwigWrap::NumList_Count(indices);

    // NOTE: If we are processing multiple indices, then the return value
    // is 1, since some indices may succeed and some may fail. So our return
    // value merely communicates: The processing was performed.
    //
    // ===> Whereas if there is only ONE index, then we need to set the return
    // value directly to the result of processing that index. Just watch
    // nReturnValue
    // to see how that is being done.
    //
    int32_t nReturnValue = 1;

    for (int32_t i = items - 1; 0 <= i; i--) {
        if (all || SwigWrap::NumList_VerifyQuery(indices, to_string(i))) {
            if (bIsDefinitelyPaymentPlan) {

                string instrument =
                    get_payment_instrument(server, mynym, i, inbox);
                if ("" == instrument) {
                    otOut << "CmdBaseAccept::acceptFromPaymentbox: "
                             "Error: cannot get payment instrument from "
                             "inpayments box.\n";
                    return -1;
                }

                CmdConfirm cmd;
                string recipient =
                    SwigWrap::Instrmnt_GetRecipientNymID(instrument);
                int32_t nTemp = cmd.confirmInstrument(
                    server,
                    mynym,
                    myacct,
                    recipient,
                    instrument,
                    i,
                    pOptionalOutput);
                if (1 == nNumlistCount) {  // If there's exactly 1 instrument
                                           // being singled-out
                    nReturnValue = nTemp;  // for processing, then return its
                                           // success/fail status.
                    break;  // Since there's only one, might as well break;
                }
            } else {
                CmdPayInvoice payInvoice;
                int32_t nTemp = payInvoice.processPayment(
                    myacct, paymentType, inbox, i, pOptionalOutput);
                if (1 == nNumlistCount) {  // If there's exactly 1 instrument
                                           // being singled-out
                    nReturnValue = nTemp;  // for processing, then return its
                                           // success/fail status.
                    break;  // Since there's only one, might as well break;
                }
            }
        }
    }

    return nReturnValue;
}
