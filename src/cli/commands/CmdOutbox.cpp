// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdOutbox.hpp"

#include <opentxs/opentxs.hpp>

#include <iostream>
#include <stdint.h>
#include <string>

#define OT_METHOD "opentxs::CmdOutbox::"

using namespace opentxs;
using namespace std;

CmdOutbox::CmdOutbox()
{
    command = "outbox";
    args[0] = "--myacct <account>";
    category = catAccounts;
    help = "Show myacct's outbox.";
}

CmdOutbox::~CmdOutbox() {}

int32_t CmdOutbox::runWithOptions() { return run(getOption("myacct")); }

int32_t CmdOutbox::run(string myacct)
{
    if (!checkAccount("myacct", myacct)) { return -1; }

    string server = SwigWrap::GetAccountWallet_NotaryID(myacct);
    if ("" == server) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot determine server from myacct.")
            .Flush();
        return -1;
    }

    string mynym = SwigWrap::GetAccountWallet_NymID(myacct);
    if ("" == mynym) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot determine mynym from myacct.")
            .Flush();
        return -1;
    }

    string outbox = SwigWrap::LoadOutbox(server, mynym, myacct);
    if ("" == outbox) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot load outbox.")
            .Flush();
        return -1;
    }

    int32_t items = SwigWrap::Ledger_GetCount(server, mynym, myacct, outbox);
    if (0 > items) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot load outbox item count.")
            .Flush();
        return -1;
    }

    if (0 == items) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": The outbox is empty.").Flush();
        return 0;
    }

    cout << "Show outbox for an asset account  (" << myacct << "): \n"
         << "Idx  Amt  Type        Txn# InRef#|User / Acct\n"
         << "---------------------------------|(from or to)\n";

    for (int32_t i = 0; i < items; i++) {
        string tx = SwigWrap::Ledger_GetTransactionByIndex(
            server, mynym, myacct, outbox, i);
        int64_t txNum = SwigWrap::Ledger_GetTransactionIDByIndex(
            server, mynym, myacct, outbox, i);
        int64_t refNum = SwigWrap::Transaction_GetDisplayReferenceToNum(
            server, mynym, myacct, tx);
        int64_t amount =
            SwigWrap::Transaction_GetAmount(server, mynym, myacct, tx);
        string type = SwigWrap::Transaction_GetType(server, mynym, myacct, tx);
        string senderUser =
            SwigWrap::Transaction_GetSenderNymID(server, mynym, myacct, tx);
        string senderAcct =
            SwigWrap::Transaction_GetSenderAcctID(server, mynym, myacct, tx);
        string recipientUser =
            SwigWrap::Transaction_GetRecipientNymID(server, mynym, myacct, tx);
        string recipientAcct =
            SwigWrap::Transaction_GetRecipientAcctID(server, mynym, myacct, tx);

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
