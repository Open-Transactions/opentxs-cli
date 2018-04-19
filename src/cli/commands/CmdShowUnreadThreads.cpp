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

#include "CmdShowUnreadThreads.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{
CmdShowUnreadThreads::CmdShowUnreadThreads()
{
    command = "showunreadthreads";
    args[0] = "--mynym <nym>";
    category = catOtherUsers;
    help =
        "List activity threads containing unread items for the specified user.";
}

std::int32_t CmdShowUnreadThreads::runWithOptions()
{
    return run(getOption("mynym"));
}

std::int32_t CmdShowUnreadThreads::run(std::string mynym)
{
    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    const auto& ot = OT::App();
    const auto& activity = ot.Activity();
    const auto threads = activity.Threads(Identifier(mynym), true);

    otOut << "Activity threads for: " << mynym << "\n";

    for (const auto& thread : threads) {
        const auto& threadID = thread.first;
        otOut << "    * " << threadID << "\n";
    }

    otOut << std::endl;

    return 1;
}
}  // namespace opentxs
