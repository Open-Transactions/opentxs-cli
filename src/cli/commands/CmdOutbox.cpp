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

#include "CmdOutbox.hpp"

#include "CmdBase.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/OTAPI_Wrap.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdOutbox::CmdOutbox()
{
    command = "outbox";
    args[0] = "--myacct <account>";
    category = catAccounts;
    help = "Show myacct's outbox.";
}

CmdOutbox::~CmdOutbox()
{
}

int32_t CmdOutbox::runWithOptions()
{
    return run(getOption("myacct"));
}

int32_t CmdOutbox::run(string myacct)
{
    if (!checkAccount("myacct", myacct)) {
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

    string outbox = OTAPI_Wrap::LoadOutbox(server, mynym, myacct);
    if ("" == outbox) {
        otOut << "Error: cannot load outbox.\n";
        return -1;
    }

    int32_t items = OTAPI_Wrap::Ledger_GetCount(server, mynym, myacct, outbox);
    if (0 > items) {
        otOut << "Error: cannot load outbox item count.\n";
        return -1;
    }

    if (0 == items) {
        otOut << "The outbox is empty.\n";
        return 0;
    }

    cout << "Show outbox for an asset account  (" << myacct << "): \n"
         << "Idx  Amt  Type        Txn# InRef#|User / Acct\n"
         << "---------------------------------|(from or to)\n";

    for (int32_t i = 0; i < items; i++) {
        string tx = OTAPI_Wrap::Ledger_GetTransactionByIndex(server, mynym,
                                                             myacct, outbox, i);
        int64_t txNum = OTAPI_Wrap::Ledger_GetTransactionIDByIndex(
            server, mynym, myacct, outbox, i);
        int64_t refNum = OTAPI_Wrap::Transaction_GetDisplayReferenceToNum(
            server, mynym, myacct, tx);
        int64_t amount =
            OTAPI_Wrap::Transaction_GetAmount(server, mynym, myacct, tx);
        string type =
            OTAPI_Wrap::Transaction_GetType(server, mynym, myacct, tx);
        string senderUser =
            OTAPI_Wrap::Transaction_GetSenderNymID(server, mynym, myacct, tx);
        string senderAcct =
            OTAPI_Wrap::Transaction_GetSenderAcctID(server, mynym, myacct, tx);
        string recipientUser = OTAPI_Wrap::Transaction_GetRecipientNymID(
            server, mynym, myacct, tx);
        string recipientAcct = OTAPI_Wrap::Transaction_GetRecipientAcctID(
            server, mynym, myacct, tx);

        string user = recipientUser;
        string acct = recipientAcct;

        bool haveUser = "" != user;
        bool haveAcct = "" != acct;

        string separator = (haveUser || haveAcct)
                               ? "\n                                 |"
                               : "";
        string userDenoter = haveUser ? "U:" : "";
        string acctDenoter = haveAcct ? "A:" : "";
        string assetType = haveAcct ? getAccountAssetType(acct) : "";

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
