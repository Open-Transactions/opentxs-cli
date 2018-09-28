// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdPingNotary.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <string>

using namespace opentxs;
using namespace std;

CmdPingNotary::CmdPingNotary()
{
    command = "pingnotary";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    category = catMisc;
    help = "See if a notary is responsive.";
}

CmdPingNotary::~CmdPingNotary() {}

int32_t CmdPingNotary::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"));
}

int32_t CmdPingNotary::run(string server, string mynym)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    auto context = Opentxs::Client().Wallet().mutable_ServerContext(
        Identifier::Factory(mynym), Identifier::Factory(server));
    const auto response = context.It().PingNotary();
    const auto& reply = response.second;
    return processResponse(String::Factory(*reply)->Get(), "ping notary");
}
