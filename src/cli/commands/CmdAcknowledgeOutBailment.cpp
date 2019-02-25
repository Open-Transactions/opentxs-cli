// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdAcknowledgeOutBailment.hpp"

#include <opentxs/opentxs.hpp>

#define OT_METHOD "opentxs::CmdAcknowledgeOutBailment"

namespace opentxs
{

CmdAcknowledgeOutBailment::CmdAcknowledgeOutBailment()
{
    command = "acknowledgeoutbailment";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--hisnym <nym>";
    args[3] = "--mypurse ID for the request being acknowledged";
    category = catOtherUsers;
    help = "Respond to an out bailment request with withdrawal instructions";
}

CmdAcknowledgeOutBailment::~CmdAcknowledgeOutBailment() {}

std::int32_t CmdAcknowledgeOutBailment::runWithOptions()
{
    return run(
        getOption("server"),
        getOption("mynym"),
        getOption("hisnym"),
        getOption("mypurse"));
}

std::int32_t CmdAcknowledgeOutBailment::run(
    std::string server,
    std::string mynym,
    std::string hisnym,
    std::string mypurse)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    if (!checkNym("hisnym", hisnym)) { return -1; }

    std::string terms = inputText("Withdrawal instructions");
    if (0 == terms.size()) { return -1; }

    auto task = Opentxs::Client().OTX().AcknowledgeOutbailment(
        identifier::Nym::Factory(mynym),
        identifier::Server::Factory(server),
        identifier::Nym::Factory(hisnym),
        Identifier::Factory(mypurse),
        terms);
    
    const auto result = std::get<1>(task).get();

    if (false == CmdBase::GetResultSuccess(result)) {
        LogOutput(OT_METHOD)(__FUNCTION__)(
            ": Failed to acknowledge outbailment")
            .Flush();
        return -1;
    }

    return 1;
}
}  // namespace opentxs
