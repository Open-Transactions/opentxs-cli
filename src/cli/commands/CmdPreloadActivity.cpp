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

#include "CmdPreloadActivity.hpp"

#include <opentxs/api/Activity.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/api/OT.hpp>
#include <opentxs/core/Identifier.hpp>

namespace opentxs
{
CmdPreloadActivity::CmdPreloadActivity()
{
    command = "preloadactivity";
    args[0] = "--mynym <nym>";
    args[1] = "--items <count>";
    category = catOtherUsers;
    help = "Cache messages in all of a nym's threads.";
}

std::int32_t CmdPreloadActivity::runWithOptions()
{
    return run(getOption("mynym"), getOption("items"));
}

std::int32_t CmdPreloadActivity::run(
    std::string mynym,
    const std::string& items)
{
    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    std::size_t count{0};

    try {
        count = std::stoul(items);
    } catch (const std::out_of_range&) {
        count = 1;
    } catch (const std::invalid_argument&) {
        count = 1;
    }

    OT::App().Activity().PreloadActivity(Identifier(mynym), count);

    return 0;
}
} // namespace opentxs
