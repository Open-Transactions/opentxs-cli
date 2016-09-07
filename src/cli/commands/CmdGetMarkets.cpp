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

#include "CmdGetMarkets.hpp"

#include "CmdBase.hpp"
#include "CmdShowMarkets.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/OT_ME.hpp>
#include <opentxs/core/Log.hpp>

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

CmdGetMarkets::~CmdGetMarkets()
{
}

int32_t CmdGetMarkets::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"));
}

int32_t CmdGetMarkets::run(string server, string mynym)
{
    if (!checkServer("server", server)) {
        return -1;
    }

    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    OT_ME ot_me;
    string response = ot_me.get_market_list(server, mynym);
    if (1 != responseStatus(response)) {
        otOut << "Error: cannot get market list.\n";
        return -1;
    }

    CmdShowMarkets showMarkets;
    return showMarkets.run(server);
}
