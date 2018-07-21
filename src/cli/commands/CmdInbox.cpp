// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdInbox.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdInbox::CmdInbox()
{
    command = "inbox";
    args[0] = "--myacct <account>";
    category = catAccounts;
    help = "Show myacct's inbox.";
}

CmdInbox::~CmdInbox() {}

int32_t CmdInbox::runWithOptions() { return run(getOption("myacct")); }

int32_t CmdInbox::run(string myacct)
{
    if (!checkAccount("myacct", myacct)) { return -1; }

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

    string inbox = SwigWrap::LoadInbox(server, mynym, myacct);
    if ("" == inbox) {
        otOut << "Error: cannot load inbox.\n";
        return -1;
    }

    int32_t items = SwigWrap::Ledger_GetCount(server, mynym, myacct, inbox);
    if (0 > items) {
        otOut << "Error: cannot load inbox item count.\n";
        return -1;
    }

    if (0 == items) {
        otOut << "The inbox is empty.\n";
        return 0;
    }

    cout << "Show inbox for an asset account  (" << myacct << "): \n"
         << "Idx  Amt  Type        Txn# InRef#|User / Acct\n"
         << "---------------------------------|(from or to)\n";

    for (int32_t i = 0; i < items; i++) {
        string tx = SwigWrap::Ledger_GetTransactionByIndex(
            server, mynym, myacct, inbox, i);
        int64_t txNum = SwigWrap::Ledger_GetTransactionIDByIndex(
            server, mynym, myacct, inbox, i);
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

        string user = "" != senderUser ? senderUser : recipientUser;
        string acct = "" != senderAcct ? senderAcct : recipientAcct;

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

    otOut << "\n For the above, try: accepttransfers, acceptreceipts, "
             "acceptinbox, acceptmoney, or acceptall.\n";

    return 1;
}
