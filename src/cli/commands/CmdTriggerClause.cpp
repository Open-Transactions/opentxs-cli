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

#include "CmdTriggerClause.hpp"

#include "CmdBase.hpp"

#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/OT.hpp>
#include <opentxs/client/OT_ME.hpp>

#include <stdint.h>
#include <string>

using namespace opentxs;
using namespace std;

CmdTriggerClause::CmdTriggerClause()
{
    command = "triggerclause";
    // FIX many more args
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--id <transactionnr>";
    args[3] = "--clause <clausename>";
    args[4] = "[--param <parameter>]";
    category = catInstruments;
    help = "Trigger a clause on a running smart contract.";
}

CmdTriggerClause::~CmdTriggerClause()
{
}

int32_t CmdTriggerClause::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("id"),
               getOption("clause"), getOption("param"));
}

// We might also need something like: OT_Command::main_show_cron_items
// (Because we can't trigger a clause on a running smart contract, unless
// we are able to list the running smart contracts and thus ascertain
// the transaction number of the one whose clause we wish to trigger.)

int32_t CmdTriggerClause::run(string server, string mynym, string id,
                              string clause, string param)
{
    if (!checkServer("server", server)) {
        return -1;
    }

    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    if (!checkTransNum("id", id)) {
        return -1;
    }

    if (!checkMandatory("clause", clause)) {
        return -1;
    }

    if ("" == param) {
        param = inputText("the clause parameter");
        if ("" == param) {
            return -1;
        }
    }


    string response =
        OT::App().API().OTME().trigger_clause(server, mynym, std::stoi(id), clause, param);
    return processResponse(response, "trigger clause");
}
