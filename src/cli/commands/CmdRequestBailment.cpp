// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdRequestBailment.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{

CmdRequestBailment::CmdRequestBailment()
{
    command = "requestbailment";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--hisnym <nym>";
    args[3] = "--mypurse <unit definition id>";
    category = catOtherUsers;
    help = "Ask the issuer of a unit to accept a deposit";
}

CmdRequestBailment::~CmdRequestBailment() {}

std::int32_t CmdRequestBailment::runWithOptions()
{
    return run(
        getOption("server"),
        getOption("mynym"),
        getOption("hisnym"),
        getOption("mypurse"));
}

std::int32_t CmdRequestBailment::run(
    std::string server,
    std::string mynym,
    std::string hisnym,
    std::string mypurse)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    if (!checkNym("hisnym", hisnym)) { return -1; }

    if (!checkPurse("mypurse", mypurse)) { return -1; }

    std::string response;
    {
        response = OT::App()
                       .Client()
                       .ServerAction()
                       .InitiateBailment(
                           Identifier::Factory(mynym),
                           Identifier::Factory(server),
                           Identifier::Factory(hisnym),
                           Identifier::Factory(mypurse))
                       ->Run();
    }
    return processResponse(response, "request bailment");
}
}  // namespace opentxs
