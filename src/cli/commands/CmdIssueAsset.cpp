// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdIssueAsset.hpp"
#include "CmdRegisterNym.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <string>

namespace opentxs
{
CmdIssueAsset::CmdIssueAsset()
{
    command = "issueasset";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--mypurse <unit definition id>";
    category = catAdmin;
    help = "Issue a currency contract onto an OT server.";
    usage = "Mynym must already be the contract key on the new contract.";
}

int32_t CmdIssueAsset::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("mypurse"));
}

int32_t CmdIssueAsset::run(
    std::string server,
    std::string mynym,
    std::string mypurse)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    if (!SwigWrap::IsNym_RegisteredAtServer(mynym, server)) {
        CmdRegisterNym registerNym;
        registerNym.run(server, mynym, "true", "false");
    }

    const auto contract = Opentxs::Client().Wallet().UnitDefinition(
        Identifier::Factory(mypurse));

    if (false == bool(contract)) { return -1; }

    std::string response;
    {
        response = Opentxs::
                       Client()
                       .ServerAction()
                       .IssueUnitDefinition(
                           Identifier::Factory(mynym),
                           Identifier::Factory(server),
                           contract->PublicContract())
                       ->Run();
    }
    return processResponse(response, "issue asset contract");
}
}  // namespace opentxs
