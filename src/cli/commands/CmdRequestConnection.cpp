// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdRequestConnection.hpp"

#include <opentxs/opentxs.hpp>

#include <stdexcept>

namespace opentxs
{

CmdRequestConnection::CmdRequestConnection()
{
    command = "requestconnection";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--hisnym <nym>";
    args[3] = "--mypurse <connection type>";
    category = catOtherUsers;
    help = "Request service connection info from another user";
}

CmdRequestConnection::~CmdRequestConnection() {}

std::int32_t CmdRequestConnection::runWithOptions()
{
    return run(
        getOption("server"),
        getOption("mynym"),
        getOption("hisnym"),
        getOption("mypurse"));
}

std::int32_t CmdRequestConnection::run(
    std::string server,
    std::string mynym,
    std::string hisnym,
    std::string mypurse)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    if (!checkNym("hisnym", hisnym)) { return -1; }

    if (mypurse.empty()) { return -1; }

    std::int64_t type{};

    try {
        type = std::stoi(mypurse);
    } catch (const std::invalid_argument&) {
        return -1;
    } catch (const std::out_of_range&) {
        return -1;
    }

    std::string response;
    {
        response = OT::App()
                       .Client()
                       .ServerAction()
                       .InitiateRequestConnection(
                           Identifier::Factory(mynym),
                           Identifier::Factory(server),
                           Identifier::Factory(hisnym),
                           proto::ConnectionInfoType(type))
                       ->Run();
    }

    return processResponse(response, "request connection");
}
}  // namespace opentxs
