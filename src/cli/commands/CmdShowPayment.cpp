// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdShowPayment.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

#define OT_METHOD "opentxs::CmdShowPayment::"

using namespace opentxs;
using namespace std;

CmdShowPayment::CmdShowPayment()
{
    command = "showpayment";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "[--index <index>]";
    args[3] = "[--showmemo <true|FALSE>]";
    category = catOtherUsers;
    help = "Show details of an incoming payment in the payments inbox.";
    usage = "Server is an Opentxs Notary where you receive incoming messages "
            "(including payments). Default index is 0.";
}

CmdShowPayment::~CmdShowPayment() {}

int32_t CmdShowPayment::runWithOptions()
{
    return run(
        getOption("server"),
        getOption("mynym"),
        getOption("index"),
        getOption("showmemo"));
}

int32_t CmdShowPayment::run(
    string server,
    string mynym,
    string index,
    string showmemo)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    if ("" != showmemo && !checkFlag("showmemo", showmemo)) { return -1; }

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

    int32_t messageNr = "" == index ? 0 : checkIndex("index", index, items);
    if (0 > messageNr) { return -1; }

    if (0 == items) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": The payment inbox is empty.")
            .Flush();
        return 0;
    }

    cout << "Idx  Amt   Type       Txn#   Memo\n";
    cout << "---------------------------------\n";

    string payment =
        SwigWrap::Ledger_GetInstrument(server, mynym, mynym, inbox, messageNr);
    if ("" == payment) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot load payment.")
            .Flush();
        return -1;
    }

    string tx = SwigWrap::Ledger_GetTransactionByIndex(
        server, mynym, mynym, inbox, messageNr);
    int64_t txNum = SwigWrap::Ledger_GetTransactionIDByIndex(
        server, mynym, mynym, inbox, messageNr);
    /* int64_t refNum = */ SwigWrap::Transaction_GetDisplayReferenceToNum(
        server, mynym, mynym, tx);
    int64_t amount = SwigWrap::Instrmnt_GetAmount(payment);
    string type = SwigWrap::Instrmnt_GetType(payment);
    string assetType = SwigWrap::Instrmnt_GetInstrumentDefinitionID(payment);
    string senderUser = SwigWrap::Instrmnt_GetSenderNymID(payment);
    string senderAcct = SwigWrap::Instrmnt_GetSenderAcctID(payment);
    string recipientUser = SwigWrap::Instrmnt_GetRecipientNymID(payment);
    string recipientAcct = SwigWrap::Instrmnt_GetRecipientAcctID(payment);
    string remitterUser = SwigWrap::Instrmnt_GetRemitterNymID(payment);
    string remitterAcct = SwigWrap::Instrmnt_GetRemitterAcctID(payment);

    string memo = SwigWrap::Instrmnt_GetMemo(payment);
    if ("" != memo) {
        if (memo.find("\n") != string::npos && showmemo != "true") {
            memo = "<too large to display here>";
        } else {
            memo = "\"" + memo + "\"";
        }
    }

    string user = "" != remitterUser ? remitterUser : senderUser;
    if ("" != user) {
        string userName = SwigWrap::GetNym_Name(user);
        if ("" != userName) user += " - \"" + userName + "\"";
        user = "U:" + user + "\n";
    }

    string acct = "" != remitterAcct ? remitterAcct : senderAcct;
    if ("" != acct) {
        string acctName = SwigWrap::GetAccountWallet_Name(acct);
        if ("" != acctName) acct += " - \"" + acctName + "\"";
        acct = "A:" + acct + "\n";
    }

    cout << messageNr << "    ";

    string fmtAmount = formatAmount(assetType, amount);
    cout << fmtAmount << (fmtAmount.size() < 3 ? "    " : "   ");

    if ("" != assetType) {
        string assetName = SwigWrap::GetAssetType_Name(assetType);
        if ("" != assetName) assetType += " - \"" + assetName + "\"";
        assetType = "T:" + assetType + "\n";
    }

    cout << type << (type.size() > 10 ? " " : "    ");
    cout << txNum << (to_string(txNum).size() < 2 ? "    " : "   ");
    cout << memo << "\n";
    cout << user;
    cout << acct;
    cout << assetType;
    cout << "Instrument:\n" << payment << "\n";

    return 1;
}
