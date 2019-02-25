// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdWithdrawVoucher.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

//#define OT_METHOD "opentxs::CmdWithdrawVoucher::"

using namespace opentxs;
using namespace std;

CmdWithdrawVoucher::CmdWithdrawVoucher()
{
    command = "withdrawvoucher";
    args[0] = "--myacct <account>";
    args[1] = "--hisnym <nym>";
    args[2] = "--amount <amount>";
    args[3] = "[--memo <memoline>]";
    category = catInstruments;
    help = "Withdraw from myacct as a voucher (cashier's cheque).";
    usage = "Use sendvoucher if you want to send it immediately.";
}

CmdWithdrawVoucher::~CmdWithdrawVoucher() {}

int32_t CmdWithdrawVoucher::runWithOptions()
{
    return run(
        getOption("myacct"),
        getOption("hisnym"),
        getOption("amount"),
        getOption("memo"));
}

int32_t CmdWithdrawVoucher::run(
    string myacct,
    string hisnym,
    string amount,
    string memo)
{
    string voucher;
    if (1 > run(myacct, hisnym, amount, memo, voucher)) { return -1; }

    cout << voucher << "\n";

    return 1;
}

int32_t CmdWithdrawVoucher::run(
    string myacct,
    string hisnym,
    string amount,
    string memo,
    string& voucher)
{
//    if (!checkAccount("myacct", myacct)) { return -1; }
//
//    if (!checkNym("hisnym", hisnym, false)) { return -1; }
//
//    int64_t value = checkAmount("amount", amount, myacct);
//    if (OT_ERROR_AMOUNT == value) { return -1; }
//
//    string server = SwigWrap::GetAccountWallet_NotaryID(myacct);
//    if ("" == server) {
//        LogNormal(OT_METHOD)(__FUNCTION__)(
//            ": Error: cannot determine server from myacct.")
//            .Flush();
//        return -1;
//    }
//
//    string mynym = SwigWrap::GetAccountWallet_NymID(myacct);
//    if ("" == mynym) {
//        LogNormal(OT_METHOD)(__FUNCTION__)(
//            ": Error: cannot determine mynym from myacct.")
//            .Flush();
//        return -1;
//    }
//
//    const OTIdentifier theNotaryID = Identifier::Factory({server}),
//                       theNymID = Identifier::Factory({mynym}),
//                       theAcctID = Identifier::Factory({myacct});
//
//    std::string response;
//    {
//        response = Opentxs::Client()
//
//                       .ServerAction()
//                       .WithdrawVoucher(
//                           theNymID,
//                           theNotaryID,
//                           theAcctID,
//                           Identifier::Factory(hisnym),
//                           value,
//                           memo)
//                       ->Run();
//    }
//    int32_t reply =
//        responseReply(response, server, mynym, myacct, "withdraw_voucher");
//    if (1 != reply) { return reply; }
//
//    string ledger = SwigWrap::Message_GetLedger(response);
//    if ("" == ledger) {
//        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot load ledger.")
//            .Flush();
//        return -1;
//    }
//
//    string tx = SwigWrap::Ledger_GetTransactionByIndex(
//        server, mynym, myacct, ledger, 0);
//    if ("" == tx) {
//        LogNormal(OT_METHOD)(__FUNCTION__)(
//            ": Error: cannot retrieve transaction.")
//            .Flush();
//        return -1;
//    }
//
//    voucher = SwigWrap::Transaction_GetVoucher(server, mynym, myacct, tx);
//    if ("" == voucher) {
//        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot load voucher.")
//            .Flush();
//        return -1;
//    }
//
//    // Save a copy in my own outpayments box. I don't want to lose this
//    // voucher since it uses one of my own transaction numbers. (If I later
//    // send the voucher to someone, OT is smart enough to remove the first
//    // copy from outpayments, when adding the second copy.)
//    //
//    // Notice how I can send an instrument to myself. This doesn't actually
//    // send anything -- it just puts a copy into my outpayments box for
//    // safe-keeping.
//    auto payment{
//        Opentxs::Client().Factory().Payment(String::Factory(voucher.c_str()))};
//
//    OT_ASSERT(false != bool(payment));
//
//    {
//        std::shared_ptr<const OTPayment> ppayment{payment.release()};
//        Opentxs::Client()
//            .ServerAction()
//            .SendPayment(theNymID, theNotaryID, theNymID, ppayment)
//            ->Run();
//        if (!Opentxs::Client().ServerAction().DownloadAccount(
//                theNymID, theNotaryID, theAcctID, true)) {
//            LogNormal(OT_METHOD)(__FUNCTION__)(
//                ": Error retrieving intermediary files for account.")
//                .Flush();
//            return -1;
//        }
//    }
//
//    return 1;
	return -1;
}
