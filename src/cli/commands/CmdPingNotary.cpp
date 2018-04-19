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
    if (!checkServer("server", server)) {
        return -1;
    }

    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    auto context = OT::App().Wallet().mutable_ServerContext(
        Identifier(mynym), Identifier(server));
    const auto response = context.It().PingNotary();
    const auto& reply = response.second;
    return processResponse(String(*reply).Get(), "ping notary");
}
