// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdStartServer.hpp"

#include <opentxs/opentxs.hpp>

#include <cstdint>
#include <string>

#define OT_METHOD "opentxs::CmdStartServer::"

using namespace opentxs;

CmdStartServer::CmdStartServer()
{
    command = "startserver";
    args[0] = "--instance <instance number>";
    category = catMisc;
    help = "Start a server for testing";
}

std::int32_t CmdStartServer::runWithOptions()
{
    return run(getOption("instance"));
}

std::int32_t CmdStartServer::run(const std::string& instance)
{
    const auto value = opentxs::String::Factory(instance)->ToLong();
    auto& server = OT::App().StartServer({}, value, true);
    LogOutput(OT_METHOD)(__FUNCTION__)(
        ": Started server ")(server.Instance())(".").Flush();

    return 0;
}
