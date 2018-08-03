// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdShowThreads.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{
CmdShowThreads::CmdShowThreads()
{
    command = "showthreads";
    args[0] = "--mynym <nym>";
    category = catOtherUsers;
    help = "List activity threads for the specified user.";
}

std::int32_t CmdShowThreads::runWithOptions()
{
    return run(getOption("mynym"));
}

std::int32_t CmdShowThreads::run(std::string mynym)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    const auto& ot = OT::App().Client();
    const auto& activity = ot.Activity();
    const auto threads = activity.Threads(Identifier::Factory(mynym), false);

    otOut << "Activity threads for: " << mynym << "\n";

    for (const auto& thread : threads) {
        const auto& threadID = thread.first;
        const auto& label = thread.second;
        otOut << "    * " << threadID << " (" << label << ")\n";
    }

    otOut << std::endl;

    return 1;
}
}  // namespace opentxs
