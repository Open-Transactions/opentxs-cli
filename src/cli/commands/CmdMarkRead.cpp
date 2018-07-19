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

#include "CmdMarkRead.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{
CmdMarkRead::CmdMarkRead()
{
    command = "markread";
    args[0] = "--mynym <nym>";
    args[1] = "--thread <threadID>";
    args[2] = "--item <itemID>";
    category = catOtherUsers;
    help = "Mark a thread item as read.";
}

std::int32_t CmdMarkRead::runWithOptions()
{
    return run(getOption("mynym"), getOption("thread"), getOption("item"));
}

std::int32_t CmdMarkRead::run(
    std::string mynym,
    const std::string& threadID,
    const std::string& itemID)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    const auto output = OT::App().Activity().MarkRead(
        Identifier::Factory(mynym),
        Identifier::Factory(threadID),
        Identifier::Factory(itemID));

    if (output) { return 0; }

    return -1;
}
}  // namespace opentxs
