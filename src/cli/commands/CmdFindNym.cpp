// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdFindNym.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{
CmdFindNym::CmdFindNym()
{
    command = "findnym";
    args[0] = "--hisnym <nym>";
    category = catOtherUsers;
    help = "Search all known servers for a nym.";
}

std::int32_t CmdFindNym::runWithOptions() { return run(getOption("hisnym")); }

std::int32_t CmdFindNym::run(std::string hisnym)
{
    if (!checkNym("hisnym", hisnym, false)) { return -1; }

    const auto response =
        Opentxs::Client().Sync().FindNym(Identifier::Factory(hisnym));

    if (String::Factory(response)->Exists()) { return 1; }

    return -1;
}
}  // namespace opentxs
