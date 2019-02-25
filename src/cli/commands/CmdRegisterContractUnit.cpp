// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdRegisterContractUnit.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <string>

#define OT_METHOD "opentxs::CmdRegisterContractUnit"

using namespace opentxs;
using namespace std;

CmdRegisterContractUnit::CmdRegisterContractUnit()
{
    command = "registercontractunit";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--hispurse <nym>";
    category = catMisc;
    help = "Upload a unit contract to a server.";
}

CmdRegisterContractUnit::~CmdRegisterContractUnit() {}

int32_t CmdRegisterContractUnit::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("hispurse"));
}

int32_t CmdRegisterContractUnit::run(
    string server,
    string mynym,
    string hispurse)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    auto task = Opentxs::Client().OTX().PublishServerContract(
        Identifier::Factory(mynym),
        Identifier::Factory(server),
        Identifier::Factory(hispurse));

    const auto result = std::get<1>(task).get();

    if (false == CmdBase::GetResultSuccess(result)) {
        LogOutput(OT_METHOD)(__FUNCTION__)(
            ": Failed to register unit definition")
            .Flush();
        return -1;
    }

    return 1;
}
