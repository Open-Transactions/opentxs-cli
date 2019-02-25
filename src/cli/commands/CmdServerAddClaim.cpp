// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdServerAddClaim.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{

CmdServerAddClaim::CmdServerAddClaim()
{
    command = "serveraddclaim";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    category = catMisc;
    help = "Request the server to add a claim to its nym credentials";
}

CmdServerAddClaim::~CmdServerAddClaim() {}

std::int32_t CmdServerAddClaim::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"));
}

std::int32_t CmdServerAddClaim::run(std::string server, std::string mynym)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    const std::string section = inputText("Section");

    if (0 == section.size()) { return -1; }

    const std::string type = inputText("Type");

    if (0 == type.size()) { return -1; }

    const std::string value = inputText("Value");

    if (0 == section.size()) { return -1; }

    const std::string strPrimary = inputText("Primary? (true or false)");
//    bool primary = true;
//
//    if ("false" == strPrimary) { primary = false; }

    std::string response;
    {
//        response = Opentxs::
//                       Client()
//                       .ServerAction()
//                       .AddServerClaim(
//                           Identifier::Factory(mynym),
//                           Identifier::Factory(server),
//                           proto::ContactSectionName(std::stoi(section)),
//                           proto::ContactItemType(std::stoi(type)),
//                           value,
//                           primary)
//                       ->Run();
    }

    return processResponse(response, "server add claim");
}
}  // namespace opentxs
