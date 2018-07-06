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

#include "CmdRegisterContractServer.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <string>

using namespace opentxs;
using namespace std;

CmdRegisterContractServer::CmdRegisterContractServer()
{
    command = "registercontractserver";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--contract <nym>";
    category = catMisc;
    help = "Upload a server contract to a server.";
    usage = "Specify the id of the server to be registered in --contract.";
}

CmdRegisterContractServer::~CmdRegisterContractServer() {}

int32_t CmdRegisterContractServer::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("contract"));
}

int32_t CmdRegisterContractServer::run(
    string server,
    string mynym,
    string contract)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    std::string response;
    {
        response =
            OT::App()
                .API()
                .ServerAction()
                .PublishServerContract(
                    Identifier::Factory(mynym), Identifier::Factory(server), Identifier::Factory(contract))
                ->Run();
    }

    return processResponse(response, "register contract");
}
