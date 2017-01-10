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

#include "CmdNotifyBailment.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/OT_ME.hpp>

namespace opentxs {

CmdNotifyBailment::CmdNotifyBailment()
{
    command = "notifybailment";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--hisnym <nym>";
    args[3] = "--mypurse <unit definition id>";
    category = catOtherUsers;
    help = "Notify a nym of a pending blockchain deposit";
}

CmdNotifyBailment::~CmdNotifyBailment()
{
}

std::int32_t CmdNotifyBailment::runWithOptions()
{
    return run(
        getOption("server"),
        getOption("mynym"),
        getOption("hisnym"),
        getOption("mypurse"));
}

std::int32_t CmdNotifyBailment::run(
    std::string server,
    std::string mynym,
    std::string hisnym,
    std::string mypurse)
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

    if (!checkPurse("mypurse", mypurse)) {
        return -1;
    }

    std::string txid = inputText("Blockchain transaction ID");

    if (0 == txid.size()) {
        return -1;
    }

     
    std::string response = OT_ME::It().notify_bailment(
        server, mynym, hisnym, mypurse, txid);
    return processResponse(response, "notify bailment");
}
} // namespace opentxs
