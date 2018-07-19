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

#include <opentxs/opentxs.hpp>

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
    const std::string& myacct,
    const std::string& indices,
    const std::int32_t itemTypeFilter) const
{
    return OTRecordList::acceptFromInbox(myacct, indices, itemTypeFilter);
}

int32_t CmdBaseAccept::acceptFromPaymentbox(
    const string& transport_notary,
    const string& myacct,
    const string& indices,
    const string& paymentType,
    string* pOptionalOutput /*=nullptr*/) const
{

    return OTRecordList::accept_from_paymentbox(
        transport_notary, myacct, indices, paymentType, pOptionalOutput);
}
