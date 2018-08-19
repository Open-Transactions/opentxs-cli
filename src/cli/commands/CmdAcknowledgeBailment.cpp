// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdAcknowledgeBailment.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{

CmdAcknowledgeBailment::CmdAcknowledgeBailment()
{
    command = "acknowledgebailment";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--hisnym <nym>";
    args[3] = "--mypurse ID for the request being acknowledged";
    category = catOtherUsers;
    help = "Respond to a bailment request with deposit instructions";
}

CmdAcknowledgeBailment::~CmdAcknowledgeBailment() {}

std::int32_t CmdAcknowledgeBailment::runWithOptions()
{
    return run(
        getOption("server"),
        getOption("mynym"),
        getOption("hisnym"),
        getOption("mypurse"));
}

std::int32_t CmdAcknowledgeBailment::run(
    std::string server,
    std::string mynym,
    std::string hisnym,
    std::string mypurse)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    if (!checkNym("hisnym", hisnym)) { return -1; }

    std::string terms = inputText("Deposit instructions");
    if (0 == terms.size()) { return -1; }

    std::string response;
    {
        response = Opentxs::
                       Client()
                       .ServerAction()
                       .AcknowledgeBailment(
                           Identifier::Factory(mynym),
                           Identifier::Factory(server),
                           Identifier::Factory(hisnym),
                           Identifier::Factory(mypurse),
                           terms)
                       ->Run();
    }
    return processResponse(response, "acknowledge bailment");
}
}  // namespace opentxs
