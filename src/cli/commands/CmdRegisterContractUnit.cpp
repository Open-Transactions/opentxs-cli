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

#include "CmdRegisterContractUnit.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <string>

using namespace opentxs;
using namespace std;

CmdRegisterContractUnit::CmdRegisterContractUnit()
{
    command = "registercontractunit";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--hispurse <nym>";
    category = catMisc;
    help = "Upload a unit contract to a server.";
}

CmdRegisterContractUnit::~CmdRegisterContractUnit() {}

int32_t CmdRegisterContractUnit::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("hispurse"));
}

int32_t CmdRegisterContractUnit::run(
    string server,
    string mynym,
    string hispurse)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    std::string response;
    {
        response =
            OT::App()
                       .API()
                       .ServerAction()
                       .PublishUnitDefinition(
                           Identifier::Factory(mynym),
                           Identifier::Factory(server),
                           Identifier::Factory(hispurse))
                       ->Run();
    }

    return processResponse(response, "register contract");
}
