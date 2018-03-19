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

#include "CmdServerAddClaim.hpp"

#include <opentxs/api/client/ServerAction.hpp>
#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/OT.hpp>
#include <opentxs/client/ServerAction.hpp>
#include <opentxs/core/Identifier.hpp>

namespace opentxs
{

CmdServerAddClaim::CmdServerAddClaim()
{
    command = "serveraddclaim";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    category = catMisc;
    help = "Request the server to add a claim to its nym credentials";
}

CmdServerAddClaim::~CmdServerAddClaim() {}

std::int32_t CmdServerAddClaim::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"));
}

std::int32_t CmdServerAddClaim::run(std::string server, std::string mynym)
{
    if (!checkServer("server", server)) {
        return -1;
    }

    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    const std::string section = inputText("Section");

    if (0 == section.size()) {

        return -1;
    }

    const std::string type = inputText("Type");

    if (0 == type.size()) {

        return -1;
    }

    const std::string value = inputText("Value");

    if (0 == section.size()) {

        return -1;
    }

    const std::string strPrimary = inputText("Primary? (true or false)");
    bool primary = true;

    if ("false" == strPrimary) {

        primary = false;
    }

    const std::string response =
        OT::App()
            .API()
            .ServerAction()
            .AddServerClaim(
                Identifier(mynym),
                Identifier(server),
                proto::ContactSectionName(std::stoi(section)),
                proto::ContactItemType(std::stoi(type)),
                value,
                primary)
            ->Run();

    return processResponse(response, "server add claim");
}
}  // namespace opentxs
