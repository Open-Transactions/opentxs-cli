/************************************************************
 *
 *                 OPEN TRANSACTIONS
 *
 *       Financial Cryptography and Digital Cash
 *       Library, Protocol, API, Server, CLI, GUI
 *
 *       -- Anonymous Numbered Accounts.
 *       -- Untraceable Digital Cash.
 *       -- Triple-Signed Receipts.
 *       -- Cheques, Vouchers, Transfers, Inboxes.
 *       -- Basket Currencies, Markets, Payment Plans.
 *       -- Signed, XML, Ricardian-style Contracts.
 *       -- Scripted smart contracts.
 *
 *  EMAIL:
 *  fellowtraveler@opentransactions.org
 *
 *  WEBSITE:
 *  http://www.opentransactions.org/
 *
 *  -----------------------------------------------------
 *
 *   LICENSE:
 *   This Source Code Form is subject to the terms of the
 *   Mozilla Public License, v. 2.0. If a copy of the MPL
 *   was not distributed with this file, You can obtain one
 *   at http://mozilla.org/MPL/2.0/.
 *
 *   DISCLAIMER:
 *   This program is distributed in the hope that it will
 *   be useful, but WITHOUT ANY WARRANTY; without even the
 *   implied warranty of MERCHANTABILITY or FITNESS FOR A
 *   PARTICULAR PURPOSE.  See the Mozilla Public License
 *   for more details.
 *
 ************************************************************/

#include "CmdGetOffers.hpp"

#include "CmdBase.hpp"
#include "CmdShowOffers.hpp"

#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/api/OT.hpp>
#include <opentxs/client/OT_ME.hpp>

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

CmdGetOffers::~CmdGetOffers()
{
}

int32_t CmdGetOffers::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("market"),
               getOption("depth"));
}

int32_t CmdGetOffers::run(string server, string mynym, string market,
                          string depth)
{
    if (!checkServer("server", server)) {
        return -1;
    }

    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    if (!checkMandatory("market", market)) {
        return -1;
    }

    if ("" == depth) {
        depth = "50";
    }


    string response =
        OT::App().API().OTME().get_market_offers(server, mynym, market, stoll(depth));
    if (1 != processResponse(response, "get market offers")) {
        return -1;
    }

    CmdShowOffers showOffers;
    return showOffers.run(server, market);
}
