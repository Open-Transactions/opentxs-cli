// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdGetMyOffers.hpp"
#include "CmdShowMyOffers.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <string>

using namespace opentxs;
using namespace std;

CmdGetMyOffers::CmdGetMyOffers()
{
    command = "getmyoffers";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    category = catMarkets;
    help = "Download mynym's list of market offers.";
}

CmdGetMyOffers::~CmdGetMyOffers() {}

int32_t CmdGetMyOffers::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"));
}

int32_t CmdGetMyOffers::run(string server, string mynym)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    std::string response;
    {
        response =
            OT::App()
                .API()
                .ServerAction()
                .DownloadNymMarketOffers(
                    Identifier::Factory(mynym), Identifier::Factory(server))
                ->Run();
    }
    if (1 != processResponse(response, "get market offers")) { return -1; }

    CmdShowMyOffers showMyOffers;
    return showMyOffers.run(server, mynym);
}
