// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdInpayments.hpp"

#include <opentxs/opentxs.hpp>

#include <iostream>
#include <stdint.h>
#include <string>

#define OT_METHOD "opentxs::CmdInpayments::"

using namespace opentxs;
using namespace std;

CmdInpayments::CmdInpayments()
{
    command = "inpayments";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    category = catOtherUsers;
    help = "Show mynym's incoming payments box.";
}

CmdInpayments::~CmdInpayments() {}

int32_t CmdInpayments::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"));
}

int32_t CmdInpayments::run(string server, string mynym)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    string inbox = SwigWrap::LoadPaymentInbox(server, mynym);
    if ("" == inbox) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot load payment inbox.")
            .Flush();
        return -1;
    }

    int32_t items = SwigWrap::Ledger_GetCount(server, mynym, mynym, inbox);
    if (0 > items) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot load payment inbox item count.")
            .Flush();
        return -1;
    }

    if (0 == items) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": The payment inbox is empty.")
            .Flush();
        return 0;
    }

    cout << "Show payments inbox (Nym/Server)\n( " << mynym << " / " << server
         << " )\n";
    cout << "Idx  Amt   Type      Txn#  Asset_Type\n";
    cout << "---------------------------------------\n";

    for (int32_t i = 0; i < items; i++) {
        string payment =
            SwigWrap::Ledger_GetInstrument(server, mynym, mynym, inbox, i);
        if ("" == payment) {
            LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot load payment.")
                .Flush();
            return -1;
        }

        string tx = SwigWrap::Ledger_GetTransactionByIndex(
            server, mynym, mynym, inbox, i);
        int64_t txNum = SwigWrap::Ledger_GetTransactionIDByIndex(
            server, mynym, mynym, inbox, i);
        /* int64_t refNum = */ SwigWrap::Transaction_GetDisplayReferenceToNum(
            server, mynym, mynym, tx);
        int64_t amount = SwigWrap::Instrmnt_GetAmount(payment);
        string type = SwigWrap::Instrmnt_GetType(payment);
        string assetType =
            SwigWrap::Instrmnt_GetInstrumentDefinitionID(payment);
        string senderUser = SwigWrap::Instrmnt_GetSenderNymID(payment);
        string senderAcct = SwigWrap::Instrmnt_GetSenderAcctID(payment);
        string recipientUser = SwigWrap::Instrmnt_GetRecipientNymID(payment);
        string recipientAcct = SwigWrap::Instrmnt_GetRecipientAcctID(payment);

        string user = "" != senderUser ? senderUser : recipientUser;
        string acct = "" != senderAcct ? senderAcct : recipientAcct;

        string fmtAmount = formatAmount(assetType, amount);

        string txID = 0 <= txNum ? to_string(txNum) : "UNKNOWN_TRANS_NUM";

        string assetName = "";
        if ("" != assetType) {
            assetName = " - \"" + SwigWrap::GetAssetType_Name(assetType) + "\"";
        }

        cout << i << "    ";
        cout << fmtAmount << (fmtAmount.size() < 3 ? "    " : "   ");
        cout << type << (type.size() > 10 ? " " : "    ");
        cout << txID << (txID.size() < 2 ? "    " : "   ");
        cout << assetType << assetName << "\n";
    }

    LogNormal(OT_METHOD)(__FUNCTION__)(
        ": For the above, try: acceptpayments, acceptinvoices, "
        "acceptmoney, or acceptall.\nEXCEPT for smart contracts and "
        "payment plans -- for those, use: opentxs confirm.")
        .Flush();

    return 1;
}
