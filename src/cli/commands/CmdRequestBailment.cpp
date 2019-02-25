// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdRequestBailment.hpp"

#include <opentxs/opentxs.hpp>

#define OT_METHOD "opentxs::CmdRequestBailment"

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

    auto task = Opentxs::Client().OTX().InitiateBailment(
        identifier::Nym::Factory(mynym),
        identifier::Server::Factory(server),
        identifier::Nym::Factory(hisnym),
        identifier::UnitDefinition::Factory(mypurse));
    
    const auto result = std::get<1>(task).get();

    if (false == CmdBase::GetResultSuccess(result)) {
        LogOutput(OT_METHOD)(__FUNCTION__)(": Failed to request bailment")
            .Flush();
        return -1;
    }

    return 1;
}
}  // namespace opentxs
