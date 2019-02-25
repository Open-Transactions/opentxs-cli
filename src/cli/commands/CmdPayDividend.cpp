// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdPayDividend.hpp"

#include <opentxs/opentxs.hpp>

#include <ostream>
#include <stdint.h>
#include <string>

#define OT_METHOD "opentxs::CmdPayDividend::"

using namespace opentxs;
using namespace std;

CmdPayDividend::CmdPayDividend()
{
    command = "paydividend";
    args[0] = "--myacct <account>";
    args[1] = "--hispurse <purse>";
    args[2] = "--amount <pershare>";
    args[3] = "[--memo <memoline>]";
    category = catMarkets;
    help = "Send dividend payout to all shareholders (in voucher form).";
}

CmdPayDividend::~CmdPayDividend() {}

int32_t CmdPayDividend::runWithOptions()
{
    return run(
        getOption("myacct"),
        getOption("hispurse"),
        getOption("amount"),
        getOption("memo"));
}

// Just like withdraw voucher...except instead of withdrawing a single voucher
// to yourself, it removes the total dividend payout from your account, and then
// divides it up amongst the shareholders, sending them EACH a voucher cheque in
// the amount of amount * number of shares owned.

int32_t CmdPayDividend::run(
    string myacct,
    string hispurse,
    string amount,
    string memo)
{
    if (!checkAccount("myacct", myacct)) { return -1; }

    if (!checkPurse("hispurse", hispurse)) { return -1; }

    int64_t value = checkAmount("amount", amount, myacct);
    if (OT_ERROR_AMOUNT == value) { return -1; }

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

    std::string response;
    {
        response = Opentxs::Client()
                       .ServerAction()
                       .PayDividend(
                           Identifier::Factory(mynym),
                           Identifier::Factory(server),
                           Identifier::Factory(hispurse),
                           Identifier::Factory(myacct),
                           memo,
                           value)
                       ->Run();
    }
    int32_t reply =
        responseReply(response, server, mynym, myacct, "pay_dividend");
    if (1 == reply) { return reply; }

    {
        auto task = Opentxs::Client().OTX().ProcessInbox(
            Identifier::Factory(mynym),
            Identifier::Factory(server),
            Identifier::Factory(myacct));

        const auto result = std::get<1>(task).get();
        
        const auto success = CmdBase::GetResultSuccess(result);
        if (false == success) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error retrieving intermediary files for account.")
                .Flush();
            return -1;
        }
    }

    return 1;
}
