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

#include "CmdShowRecords.hpp"

#include "CmdBase.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/SwigWrap.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdShowRecords::CmdShowRecords()
{
    command = "showrecords";
    args[0] = "[--server <server>]";
    args[1] = "[--mynym <nym>]";
    args[2] = "[--myacct <account>]";
    category = catMisc;
    help = "Show contents of record box.";
    usage = "Specify either one of --server/--mynym and --myacct.";
}

CmdShowRecords::~CmdShowRecords()
{
}

int32_t CmdShowRecords::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("myacct"));
}

int32_t CmdShowRecords::run(string server, string mynym, string myacct)
{
    if ("" == myacct) {
        if (!checkServer("server", server)) {
            return -1;
        }

        if (!checkNym("mynym", mynym)) {
            return -1;
        }
    }
    else {
        if (!checkAccount("myacct", myacct)) {
            return -1;
        }

        server = SwigWrap::GetAccountWallet_NotaryID(myacct);
        if ("" == server) {
            otOut << "Error: cannot determine server from myacct.\n";
            return -1;
        }

        mynym = SwigWrap::GetAccountWallet_NymID(myacct);
        if ("" == mynym) {
            otOut << "Error: cannot determine mynym from myacct.\n";
            return -1;
        }
    }

    // REMEMBER, recordbox for myacct contains the old inbox receipts.
    // Whereas recordbox for MyNym contains the old payments (in and out.)
    // In the case of the latter, the NYM must be passed as the ACCT...
    //
    // Meaning: there are TWO record boxes. So must there thus be two commands
    // for viewing them? Or do we combine them somehow?
    ///
    // ===> I say combine them, since they are for viewing only (nothing is
    // actually done with the records -- they're just there for the actual
    // client to take and store however it wishes.)

    otOut << "Archived Nym-related records (" << mynym << "):\n";
    bool success = 0 <= showRecords(server, mynym, mynym);

    if ("" != myacct) {
        dashLine();
        otOut << "Archived Account-related records (" << myacct << "):\n";
        success |= 0 <= showRecords(server, mynym, myacct);
    }

    return success ? 1 : -1;
}

int32_t CmdShowRecords::showRecords(const string& server, const string& mynym,
                                    const string& myacct)
{
    string records = SwigWrap::LoadRecordBox(server, mynym, myacct);
    if ("" == records) {
        otOut << "Error: cannot load record box.\n";
        return -1;
    }

    int32_t items = SwigWrap::Ledger_GetCount(server, mynym, myacct, records);
    if (0 > items) {
        otOut << "Error: cannot load record box item count.\n";
        return -1;
    }

    if (0 == items) {
        otOut << "The record box is empty.\n";
        return 0;
    }

    cout << "Idx  Amt  Type        Txn# InRef#|User / Acct\n";
    cout << "---------------------------------|(from or to)\n";
    for (int32_t i = 0; i < items; i++) {
        string tx = SwigWrap::Ledger_GetTransactionByIndex(
            server, mynym, myacct, records, i);
        int64_t txNum = SwigWrap::Ledger_GetTransactionIDByIndex(
            server, mynym, myacct, records, i);
        int64_t refNum = SwigWrap::Transaction_GetDisplayReferenceToNum(
            server, mynym, myacct, tx);
        int64_t amount =
            SwigWrap::Transaction_GetAmount(server, mynym, myacct, tx);
        string type =
            SwigWrap::Transaction_GetType(server, mynym, myacct, tx);
        string senderUser =
            SwigWrap::Transaction_GetSenderNymID(server, mynym, myacct, tx);
        string senderAcct =
            SwigWrap::Transaction_GetSenderAcctID(server, mynym, myacct, tx);
        string recipientUser = SwigWrap::Transaction_GetRecipientNymID(
            server, mynym, myacct, tx);
        string recipientAcct = SwigWrap::Transaction_GetRecipientAcctID(
            server, mynym, myacct, tx);

        string user = "" != senderUser ? senderUser : recipientUser;
        string acct = "" != senderAcct ? senderAcct : recipientAcct;

        string separator = ("" == user && "" == acct)
                               ? ""
                               : "\n                                 |";

        string userDenoter = "" != user ? "U:" : "";
        string acctDenoter = "" != acct ? "A:" : "";

        string assetType = "" != acct ? getAccountAssetType(acct) : "";
        string fmtAmount = formatAmount(assetType, amount);

        cout << i << "    ";
        cout << fmtAmount << (fmtAmount.size() < 3 ? "    " : "   ");
        cout << type << (type.size() > 10 ? " " : "    ");
        cout << txNum << (to_string(txNum).size() < 2 ? "    " : "   ");
        cout << refNum << (to_string(refNum).size() > 2 ? "  " : " ") << "|";
        cout << userDenoter << user << separator << acctDenoter << acct << "\n";
    }

    return 1;
}
