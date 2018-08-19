// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdGetOffers.hpp"
#include "CmdShowOffers.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <string>

using namespace opentxs;
using namespace std;

CmdGetOffers::CmdGetOffers()
{
    command = "getoffers";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--market <marketid>";
    args[3] = "[--depth <depth>]";
    category = catMarkets;
    help = "Download mynym's list of market offers.";
    usage = "Default depth is 50";
}

CmdGetOffers::~CmdGetOffers() {}

int32_t CmdGetOffers::runWithOptions()
{
    return run(
        getOption("server"),
        getOption("mynym"),
        getOption("market"),
        getOption("depth"));
}

int32_t CmdGetOffers::run(
    string server,
    string mynym,
    string market,
    string depth)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    if (!checkMandatory("market", market)) { return -1; }

    if ("" == depth) { depth = "50"; }

    std::string response;
    {
        response = Opentxs::
                       Client()
                       .ServerAction()
                       .DownloadMarketOffers(
                           Identifier::Factory(mynym),
                           Identifier::Factory(server),
                           Identifier::Factory(market),
                           stoll(depth))
                       ->Run();
    }
    if (1 != processResponse(response, "get market offers")) { return -1; }

    CmdShowOffers showOffers;
    return showOffers.run(server, market);
}
