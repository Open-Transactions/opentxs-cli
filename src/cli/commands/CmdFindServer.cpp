// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdFindServer.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{
CmdFindServer::CmdFindServer()
{
    command = "findserver";
    args[0] = "--server <server id>";
    category = catOtherUsers;
    help = "Search all known servers for a server contract.";
}

std::int32_t CmdFindServer::runWithOptions()
{
    return run(getOption("server"));
}

std::int32_t CmdFindServer::run(std::string server)
{
    const auto response =
        OT::App().API().Sync().FindServer(Identifier::Factory(server));

    if (String(response).Exists()) { return 1; }

    return -1;
}
}  // namespace opentxs
