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

#include "CmdShowThreads.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/api/Activity.hpp>
#include <opentxs/api/OT.hpp>
#include <opentxs/core/Identifier.hpp>
#include <opentxs/core/Log.hpp>

namespace opentxs
{
CmdShowThreads::CmdShowThreads()
{
    command = "showthreads";
    args[0] = "--mynym <nym>";
    category = catOtherUsers;
    help = "List activity threads for the specified user.";
}

std::int32_t CmdShowThreads::runWithOptions()
{
    return run(getOption("mynym"));
}

std::int32_t CmdShowThreads::run(std::string mynym)
{
    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    const auto& ot = OT::App();
    const auto& activity = ot.Activity();
    const auto threads = activity.Threads(Identifier(mynym), false);

    otOut << "Activity threads for: " << mynym << "\n";

    for (const auto& thread : threads) {
        const auto& threadID = thread.first;
        otOut << "    * " << threadID << "\n";
    }

    otOut << std::endl;

    return 1;
}
} // namespace opentxs
