// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdUsageCredits.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

#define OT_METHOD "opentxs::CmdUsageCredits"

using namespace opentxs;
using namespace std;

CmdUsageCredits::CmdUsageCredits()
{
    command = "usagecredits";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--hisnym <nym>";
    args[3] = "--adjust <usagecredits>";
    category = catOtherUsers;
    help = "Give or take away hisnym's usage credits.";
    usage = "Mynym can use this on himself, read-only.";
}

CmdUsageCredits::~CmdUsageCredits() {}

int32_t CmdUsageCredits::runWithOptions()
{
    return run(
        getOption("server"),
        getOption("mynym"),
        getOption("hisnym"),
        getOption("adjust"));
}

int32_t CmdUsageCredits::run(
    string server,
    string mynym,
    string hisnym,
    string adjust)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    if (!checkNym("hisnym", hisnym)) { return -1; }

    string positiveAdjust = '-' == adjust[0] ? adjust.substr(1) : adjust;
    if (!checkValue("adjust", positiveAdjust)) { return -1; }

    std::string response;
    {
        response = Opentxs::Client()
                       .ServerAction()
                       .AdjustUsageCredits(
                           Identifier::Factory(mynym),
                           Identifier::Factory(server),
                           Identifier::Factory(hisnym),
                           std::stoll(adjust))
                       ->Run();
    }
    if (1 != processResponse(response, "adjust usage credits")) { return -1; }

    int64_t balance = SwigWrap::Message_GetUsageCredits(response);
    if (-1 > balance) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: failed to retrieve usage credits.")
            .Flush();
        return -1;
    }

    if (-1 == balance) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Nym has unlimited usage credits, or server enforcement is "
            "turned off.")
            .Flush();
        return 1;
    }

    if (0 == balance) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Nym has exhausted his usage credits.")
            .Flush();
        return 1;
    }

    LogNormal(OT_METHOD)(__FUNCTION__)(": Nym currently has ")(balance)(
        " usage credits.")
        .Flush();
    return 1;
}
