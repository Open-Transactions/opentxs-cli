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

#include "CmdKillPlan.hpp"

#include "CmdBase.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/OT_ME.hpp>

#include <stdint.h>
#include <cinttypes>
#include <cstdio>
#include <string>

using namespace opentxs;
using namespace std;

CmdKillPlan::CmdKillPlan()
{
    command = "killplan";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--myacct <account>";
    args[3] = "--id <transactionnr>";
    category = catInstruments;
    help = "Kill an active payment plan.";
}

CmdKillPlan::~CmdKillPlan()
{
}

int32_t CmdKillPlan::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("myacct"),
               getOption("id"));
}

int32_t CmdKillPlan::run(string server, string mynym, string myacct, string id)
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

     
    int64_t i;
    sscanf(id.c_str(), "%" SCNd64, &i);
    string response = OT_ME::It().kill_payment_plan(server, mynym, myacct, i);
    return processTxResponse(server, mynym, myacct, response,
                             "kill payment plan");
}
