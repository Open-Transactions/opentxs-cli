// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdShowUnreadThreads.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{
CmdShowUnreadThreads::CmdShowUnreadThreads()
{
    command = "showunreadthreads";
    args[0] = "--mynym <nym>";
    category = catOtherUsers;
    help =
        "List activity threads containing unread items for the specified user.";
}

std::int32_t CmdShowUnreadThreads::runWithOptions()
{
    return run(getOption("mynym"));
}

std::int32_t CmdShowUnreadThreads::run(std::string mynym)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    const auto& ot = OT::App().Client();
    const auto& activity = ot.Activity();
    const auto threads = activity.Threads(Identifier::Factory(mynym), true);

    otOut << "Activity threads for: " << mynym << "\n";

    for (const auto& thread : threads) {
        const auto& threadID = thread.first;
        otOut << "    * " << threadID << "\n";
    }

    otOut << std::endl;

    return 1;
}
}  // namespace opentxs
