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

#include "CmdMarkUnRead.hpp"

#include <opentxs/api/Activity.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/api/OT.hpp>
#include <opentxs/core/Identifier.hpp>

namespace opentxs
{
CmdMarkUnRead::CmdMarkUnRead()
{
    command = "markunread";
    args[0] = "--mynym <nym>";
    args[1] = "--thread <threadID>";
    args[2] = "--item <itemID>";
    category = catOtherUsers;
    help = "Mark a thread item as unread.";
}

std::int32_t CmdMarkUnRead::runWithOptions()
{
    return run(getOption("mynym"), getOption("thread"), getOption("item"));
}

std::int32_t CmdMarkUnRead::run(
    std::string mynym,
    const std::string& threadID,
    const std::string& itemID)
{
    if (!checkNym("mynym", mynym)) {

        return -1;
    }

    const auto output = OT::App().Activity().MarkUnread(
        Identifier(mynym), Identifier(threadID), Identifier(itemID));

    if (output) {

        return 0;
    }

    return -1;
}
} // namespace opentxs
