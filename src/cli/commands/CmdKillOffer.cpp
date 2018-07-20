// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdKillOffer.hpp"

#include <opentxs/opentxs.hpp>

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string>

using namespace opentxs;
using namespace std;

CmdKillOffer::CmdKillOffer()
{
    command = "killoffer";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--myacct <account>";
    args[3] = "--id <transactionnr>";
    category = catMarkets;
    help = "Kill an active market offer.";
}

CmdKillOffer::~CmdKillOffer() {}

int32_t CmdKillOffer::runWithOptions()
{
    return run(
        getOption("server"),
        getOption("mynym"),
        getOption("myacct"),
        getOption("id"));
}

int32_t CmdKillOffer::run(string server, string mynym, string myacct, string id)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    if (!checkAccount("myacct", myacct)) { return -1; }

    if (!checkTransNum("id", id)) { return -1; }

    int64_t i;
    sscanf(id.c_str(), "%" SCNd64, &i);
    std::string response;
    {
        response = OT::App()
                       .API()
                       .ServerAction()
                       .KillMarketOffer(
                           Identifier::Factory(mynym),
                           Identifier::Factory(server),
                           Identifier::Factory(myacct),
                           i)
                       ->Run();
    }
    return processTxResponse(
        server, mynym, myacct, response, "kill market offer");
}
