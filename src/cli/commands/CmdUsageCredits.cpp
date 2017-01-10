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

#include "CmdUsageCredits.hpp"

#include "CmdBase.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/OTAPI_Wrap.hpp>
#include <opentxs/client/OT_ME.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdUsageCredits::CmdUsageCredits()
{
    command = "usagecredits";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--hisnym <nym>";
    args[3] = "--adjust <usagecredits>";
    category = catOtherUsers;
    help = "Give or take away hisnym's usage credits.";
    usage = "Mynym can use this on himself, read-only.";
}

CmdUsageCredits::~CmdUsageCredits()
{
}

int32_t CmdUsageCredits::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("hisnym"),
               getOption("adjust"));
}

int32_t CmdUsageCredits::run(string server, string mynym, string hisnym,
                             string adjust)
{
    if (!checkServer("server", server)) {
        return -1;
    }

    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    if (!checkNym("hisnym", hisnym)) {
        return -1;
    }

    string positiveAdjust = '-' == adjust[0] ? adjust.substr(1) : adjust;
    if (!checkValue("adjust", positiveAdjust)) {
        return -1;
    }

     
    string response = OT_ME::It().adjust_usage_credits(server, mynym, hisnym, adjust);
    if (1 != processResponse(response, "adjust usage credits")) {
        return -1;
    }

    int64_t balance = OTAPI_Wrap::Message_GetUsageCredits(response);
    if (-1 > balance) {
        otOut << "Error: failed to retrieve usage credits.\n";
        return -1;
    }

    if (-1 == balance) {
        otOut << "Nym has unlimited usage credits, or server enforcement is "
                 "turned off.\n";
        return 1;
    }

    if (0 == balance) {
        otOut << "Nym has exhausted his usage credits.\n";
        return 1;
    }

    otOut << "Nym currently has " << balance << " usage credits.";
    return 1;
}
