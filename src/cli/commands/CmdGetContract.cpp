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

#include "CmdGetContract.hpp"

#include "CmdBase.hpp"

#include <opentxs/api/client/ServerAction.hpp>
#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/client/ServerAction.hpp>
#include <opentxs/core/Identifier.hpp>
#include <opentxs/OT.hpp>

#include <stdint.h>
#include <string>

using namespace opentxs;
using namespace std;

CmdGetInstrumentDefinition::CmdGetInstrumentDefinition()
{
    command = "getcontract";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--contract <contractid>";
    category = catAdmin;
    help = "Download an asset or server contract by its ID.";
}

CmdGetInstrumentDefinition::~CmdGetInstrumentDefinition() {}

int32_t CmdGetInstrumentDefinition::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("contract"));
}

int32_t CmdGetInstrumentDefinition::run(
    string server,
    string mynym,
    string contract)
{
    if (!checkServer("server", server)) {
        return -1;
    }

    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    if (!checkMandatory("contract", contract)) {
        return -1;
    }

    string response =
        OT::App()
            .API()
            .ServerAction()
            .DownloadContract(
                Identifier(mynym), Identifier(server), Identifier(contract))
            ->Run();
    return processResponse(response, "retrieve contract");
}
