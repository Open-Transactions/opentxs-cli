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

#include "CmdFindServer.hpp"

#include "CmdBase.hpp"

#include <opentxs/api/client/Sync.hpp>
#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/core/Identifier.hpp>
#include <opentxs/OT.hpp>

namespace opentxs
{
CmdFindServer::CmdFindServer()
{
    command = "findserver";
    args[0] = "--server <server id>";
    category = catOtherUsers;
    help = "Search all known servers for a server contract.";
}

std::int32_t CmdFindServer::runWithOptions()
{
    return run(getOption("server"));
}

std::int32_t CmdFindServer::run(std::string server)
{
    const auto response = OT::App().API().Sync().FindServer(Identifier(server));

    if (String(response).Exists()) { return 1; }

    return -1;
}
} // namespace opentxs
