// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdGetContract.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <string>

using namespace opentxs;
using namespace std;

CmdGetInstrumentDefinition::CmdGetInstrumentDefinition()
{
    command = "getcontract";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--contract <contractid>";
    category = catAdmin;
    help = "Download an asset or server contract by its ID.";
}

CmdGetInstrumentDefinition::~CmdGetInstrumentDefinition() {}

int32_t CmdGetInstrumentDefinition::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("contract"));
}

int32_t CmdGetInstrumentDefinition::run(
    string server,
    string mynym,
    string contract)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    if (!checkMandatory("contract", contract)) { return -1; }

    std::string response;
    {
        response = OT::App()
                       .API()
                       .ServerAction()
                       .DownloadContract(
                           Identifier::Factory(mynym),
                           Identifier::Factory(server),
                           Identifier::Factory(contract))
                       ->Run();
    }
    return processResponse(response, "retrieve contract");
}
