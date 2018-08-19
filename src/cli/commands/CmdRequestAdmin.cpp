// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdRequestAdmin.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{

CmdRequestAdmin::CmdRequestAdmin()
{
    command = "requestadmin";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--hisnym <server password>";
    category = catMisc;
    help = "Request to become the admin nym for a server";
}

CmdRequestAdmin::~CmdRequestAdmin() {}

std::int32_t CmdRequestAdmin::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("hisnym"));
}

std::int32_t CmdRequestAdmin::run(
    std::string server,
    std::string mynym,
    std::string hisnym)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    if (hisnym.empty()) { return -1; }

    std::string response;
    {
        response = Opentxs::
                       Client()
                       .ServerAction()
                       .RequestAdmin(
                           Identifier::Factory(mynym),
                           Identifier::Factory(server),
                           hisnym)
                       ->Run();
    }

    return processResponse(response, "request admin");
}
}  // namespace opentxs
