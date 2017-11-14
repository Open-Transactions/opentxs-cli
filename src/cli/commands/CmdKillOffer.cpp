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

#include "CmdKillOffer.hpp"

#include "CmdBase.hpp"

#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/api/OT.hpp>
#include <opentxs/client/OT_ME.hpp>

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

CmdKillOffer::~CmdKillOffer()
{
}

int32_t CmdKillOffer::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("myacct"),
               getOption("id"));
}

int32_t CmdKillOffer::run(string server, string mynym, string myacct, string id)
{
    if (!checkServer("server", server)) {
        return -1;
    }

    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    if (!checkAccount("myacct", myacct)) {
        return -1;
    }

    if (!checkTransNum("id", id)) {
        return -1;
    }


    int64_t i;
    sscanf(id.c_str(), "%" SCNd64, &i);
    string response = OT::App().API().OTME().kill_market_offer(server, mynym, myacct, i);
    return processTxResponse(server, mynym, myacct, response,
                             "kill market offer");
}
