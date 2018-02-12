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

#include "CmdRegisterNym.hpp"

#include "CmdBase.hpp"

#include <opentxs/api/client/Sync.hpp>
#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/core/Identifier.hpp>
#include <opentxs/OT.hpp>

namespace opentxs
{

CmdRegisterNym::CmdRegisterNym()
{
    command = "registernym";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    category = catAdmin;
    help = "Register mynym onto an OT server.";
}

std::int32_t CmdRegisterNym::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"));
}

std::int32_t CmdRegisterNym::run(std::string server, std::string mynym)
{
    if (!checkServer("server", server)) {
        return -1;
    }

    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    const auto response =
        OT::App().API().Sync().RegisterNym(Identifier(mynym), Identifier(server), true);

    if (false == response.empty()) {

        return 0;
    } else {

        return -1;
    }
}
} // namespace opentxs
