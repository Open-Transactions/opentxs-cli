// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdGetMarkets.hpp"
#include "CmdShowMarkets.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdGetMarkets::CmdGetMarkets()
{
    command = "getmarkets";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    category = catMarkets;
    help = "Download mynym's list of markets.";
}

CmdGetMarkets::~CmdGetMarkets() {}

int32_t CmdGetMarkets::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"));
}

int32_t CmdGetMarkets::run(string server, string mynym)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    std::string response;
    {
        response =
            OT::App()
                .API()
                .ServerAction()
                .DownloadMarketList(
                    Identifier::Factory(mynym), Identifier::Factory(server))
                ->Run();
    }
    if (1 != responseStatus(response)) {
        otOut << "Error: cannot get market list.\n";
        return -1;
    }

    CmdShowMarkets showMarkets;
    return showMarkets.run(server);
}
