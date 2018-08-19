// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdRequestOutBailment.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{

CmdRequestOutBailment::CmdRequestOutBailment()
{
    command = "requestoutbailment";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--hisnym <nym>";
    args[3] = "--mypurse <purse>";
    args[4] = "--amount <amount>";
    category = catOtherUsers;
    help = "Ask the issuer of a unit to process a withdrawal";
}

CmdRequestOutBailment::~CmdRequestOutBailment() {}

std::int32_t CmdRequestOutBailment::runWithOptions()
{
    return run(
        getOption("server"),
        getOption("mynym"),
        getOption("hisnym"),
        getOption("mypurse"),
        getOption("amount"));
}

std::int32_t CmdRequestOutBailment::run(
    std::string server,
    std::string mynym,
    std::string hisnym,
    std::string mypurse,
    std::string amount)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    if (!checkNym("hisnym", hisnym)) { return -1; }

    if (!checkPurse("mypurse", mypurse)) { return -1; }

    std::string terms = inputText("Withdrawal instructions");
    if (0 == terms.size()) { return -1; }

    std::int64_t outbailmentAmount = SwigWrap::StringToAmount(mypurse, amount);
    if (OT_ERROR_AMOUNT == outbailmentAmount) { return -1; }

    std::string response;
    {
        response = Opentxs::
                       Client()
                       .ServerAction()
                       .InitiateOutbailment(
                           Identifier::Factory(mynym),
                           Identifier::Factory(server),
                           Identifier::Factory(hisnym),
                           Identifier::Factory(mypurse),
                           outbailmentAmount,
                           terms)
                       ->Run();
    }
    return processResponse(response, "request outbailment");
}
}  // namespace opentxs
