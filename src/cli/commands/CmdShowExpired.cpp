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

#include "CmdShowExpired.hpp"

#include "CmdBase.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/SwigWrap.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdShowExpired::CmdShowExpired()
{
    command = "showexpired";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    category = catMisc;
    help = "Show mynym's expired record box.";
}

CmdShowExpired::~CmdShowExpired()
{
}

int32_t CmdShowExpired::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"));
}

// TODO: This is based on details_show_record which assumes it can contain asset
// inbox records as well as payment inbox records. But in fact, the expired box
// can ONLY contain payment records, not asset account records. So at some
// point, we will want to update this function to use the Instrument calls
// instead of the typical transaction calls, since that is more appropriate in
// this case. In fact we will also want to update details_show_record to do the
// same thing, in cases where the nymID and the account ID are the same.

int32_t CmdShowExpired::run(string server, string mynym)
{
    if (!checkServer("server", server)) {
        return -1;
    }

    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    string expired = SwigWrap::LoadExpiredBox(server, mynym);
    if ("" == expired) {
        otOut << "Error: cannot load expired record box.\n";
        return -1;
    }

    int32_t items = SwigWrap::Ledger_GetCount(server, mynym, mynym, expired);
    if (0 > items) {
        otOut << "Error: cannot load expired record box item count.\n";
        return -1;
    }

    if (0 == items) {
        otOut << "The expired record box is empty.\n";
        return 0;
    }

    cout << "Expired records:\n";
    cout << "Idx  Amt  Type        Txn# InRef#|User / Acct\n";
    cout << "---------------------------------|(from or to)\n";

    for (int32_t i = 0; i < items; i++) {
        string tx = SwigWrap::Ledger_GetTransactionByIndex(server, mynym,
                                                             mynym, expired, i);
        int64_t txNum = SwigWrap::Ledger_GetTransactionIDByIndex(
            server, mynym, mynym, expired, i);
        int64_t refNum = SwigWrap::Transaction_GetDisplayReferenceToNum(
            server, mynym, mynym, tx);
        int64_t amount =
            SwigWrap::Transaction_GetAmount(server, mynym, mynym, tx);
        string type = SwigWrap::Transaction_GetType(server, mynym, mynym, tx);
        string senderUser =
            SwigWrap::Transaction_GetSenderNymID(server, mynym, mynym, tx);
        string senderAcct =
            SwigWrap::Transaction_GetSenderAcctID(server, mynym, mynym, tx);
        string recipientUser =
            SwigWrap::Transaction_GetRecipientNymID(server, mynym, mynym, tx);
        string recipientAcct = SwigWrap::Transaction_GetRecipientAcctID(
            server, mynym, mynym, tx);

        string user = "" != senderUser ? senderUser : recipientUser;
        string acct = "" != senderAcct ? senderAcct : recipientAcct;

        string separator = "";
        if ("" != user || "" != acct) {
            separator = "\n                                 |";
        }

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
