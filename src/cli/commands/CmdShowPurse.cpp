// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdShowPurse.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdShowPurse::CmdShowPurse()
{
    command = "showpurse";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--mypurse <purse>";
    category = catWallet;
    help = "Show contents of a cash purse.";
}

CmdShowPurse::~CmdShowPurse() {}

int32_t CmdShowPurse::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("mypurse"));
}

int32_t CmdShowPurse::run(string server, string mynym, string mypurse)
{
#if OT_CASH
#define OT_METHOD "opentxs::CmdShowPurse::"

    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    if (!checkPurse("mypurse", mypurse)) { return -1; }

    string purse = SwigWrap::LoadPurse(server, mypurse, mynym);
    if ("" == purse) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannt load purse.")
            .Flush();
        return -1;
    }

    int64_t amount = SwigWrap::Purse_GetTotalValue(server, mypurse, purse);
    cout << "Total value: " << SwigWrap::FormatAmount(mypurse, amount) << "\n";

    int32_t items = SwigWrap::Purse_Count(server, mypurse, purse);
    if (0 > items) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot load purse item count.")
            .Flush();
        return -1;
    }

    if (0 == items) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": The purse is empty.").Flush();
        return 0;
    }

    time64_t now = SwigWrap::GetTime();
    if (OT_TIME_ZERO > now) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot get current time.")
            .Flush();
        return -1;
    }

    cout << "Token count: " << items << "\n";
    cout << "Index\tValue\tSeries\tValidFrom\tValidTo\t\tStatus\n";

    for (int32_t i = 0; i < items; i++) {
        string token = SwigWrap::Purse_Peek(server, mypurse, mynym, purse);
        if ("" == token) {
            LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot load token ")(
                i)(".")
                .Flush();
            return -1;
        }

        purse = SwigWrap::Purse_Pop(server, mypurse, mynym, purse);
        if ("" == purse) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: cannot load updated purse.")
                .Flush();
            return -1;
        }

        int64_t denomination =
            SwigWrap::Token_GetDenomination(server, mypurse, token);
        if (0 > denomination) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: cannot load denomination.")
                .Flush();
            return -1;
        }

        int32_t series = SwigWrap::Token_GetSeries(server, mypurse, token);
        if (0 > series) {
            LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot load series.")
                .Flush();
            return -1;
        }

        time64_t from = SwigWrap::Token_GetValidFrom(server, mypurse, token);
        if (OT_TIME_ZERO > from) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: cannot load validFrom.")
                .Flush();
            return -1;
        }

        time64_t until = SwigWrap::Token_GetValidTo(server, mypurse, token);
        if (OT_TIME_ZERO > until) {
            LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot load validTo.")
                .Flush();
            return -1;
        }

        string status = until < now ? "expired" : "valid";

        cout << i << "\t" << denomination << "\t" << series << "\t" << from
             << "\t" << until << "\t" << status << "\n";
    }

    return 1;
#else
    return -1;
#endif  // OT_CASH
}
