// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdActivitySummary.hpp"

#include <opentxs/opentxs.hpp>

#include <ctime>
#include <iostream>

#define OT_METHOD "opentxs::CmdActivitySummary::"

namespace opentxs
{
CmdActivitySummary::CmdActivitySummary()
{
    command = "activitysummary";
    args[0] = "--mynym <nym>";
    category = catOtherUsers;
    help = "Display a summary of activity for a nym.";
}

std::int32_t CmdActivitySummary::runWithOptions()
{
    return run(getOption("mynym"));
}

std::int32_t CmdActivitySummary::run(std::string mynym)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    const OTIdentifier nymID = Identifier::Factory(mynym);
    auto& activity = Opentxs::Client().UI().ActivitySummary(nymID);
    LogNormal(OT_METHOD)(__FUNCTION__)(": Activity: ").Flush();
    dashLine();
    auto line = activity.First();

    if (false == line->Valid()) { return 1; }

    auto last = line->Last();
    {
        LogNormal(OT_METHOD)(__FUNCTION__)(": * ")(line->DisplayName())(" (")(
            line->ThreadID())("): ")(time(line->Timestamp())).Flush();
	LogNormal(OT_METHOD)(__FUNCTION__)(
	   " ")(line->Text())
           .Flush();
    }
    while (false == last) {
        line = activity.Next();
        last = line->Last();
        LogNormal(OT_METHOD)(__FUNCTION__)(": * ")(line->DisplayName())(" (")(
            line->ThreadID())("): ")(time(line->Timestamp())).Flush();
	LogNormal(OT_METHOD)(__FUNCTION__)(
	   " ")(line->Text())
           .Flush();
    }

    LogNormal(OT_METHOD)(__FUNCTION__)(" ").Flush();

    return 1;
}

std::string CmdActivitySummary::time(
    const std::chrono::system_clock::time_point in) const
{
    const auto converted = std::chrono::system_clock::to_time_t(in);

    return std::ctime(&converted);
}
}  // namespace opentxs
