// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdStartIntroductionServer.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{
CmdStartIntroductionServer::CmdStartIntroductionServer()
{
    command = "startintroductionserver";
    args[0] = "--mynym <nym>";
    category = catWallet;
    help = "Initialize state machine for the introduction server";
}

std::int32_t CmdStartIntroductionServer::runWithOptions()
{
    return run(getOption("mynym"));
}

std::int32_t CmdStartIntroductionServer::run(std::string mynym)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    Opentxs::Client().Sync().StartIntroductionServer(
        Identifier::Factory(mynym));

    return 1;
}
}  // namespace opentxs
