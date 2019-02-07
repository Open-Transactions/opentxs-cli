// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdAcknowledgeConnection.hpp"

#include <opentxs/opentxs.hpp>

#define OT_METHOD "opentxs::CmdAcknowledgeConnection"

namespace opentxs
{

CmdAcknowledgeConnection::CmdAcknowledgeConnection()
{
    command = "acknowledgeconnection";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--hisnym <nym>";
    args[3] = "--mypurse ID for the request being acknowledged";
    category = catOtherUsers;
    help = "Respond to a connection request";
}

std::int32_t CmdAcknowledgeConnection::runWithOptions()
{
    return run(
        getOption("server"),
        getOption("mynym"),
        getOption("hisnym"),
        getOption("mypurse"));
}

std::int32_t CmdAcknowledgeConnection::run(
    std::string server,
    std::string mynym,
    std::string hisnym,
    std::string mypurse)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    if (!checkNym("hisnym", hisnym)) { return -1; }

    std::string url = inputText("URL");
    std::string login = inputText("User name");
    std::string password = inputText("Password");
    std::string key = inputText("Key");

    auto task = Opentxs::Client().OTX().AcknowledgeConnection(
        identifier::Nym::Factory(mynym),
        identifier::Server::Factory(server),
        identifier::Nym::Factory(hisnym),
        Identifier::Factory(mypurse),
        true,
        url,
        login,
        password,
        key);

    const auto result = std::get<1>(task).get();

    if (false == CmdBase::GetResultSuccess(result)) {
        LogOutput(OT_METHOD)(__FUNCTION__)(": Failed to acknowledge connection")
            .Flush();
        return -1;
    }

    return 1;
}
}  // namespace opentxs
