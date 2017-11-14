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

#include "CmdCanMessage.hpp"

#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/api/OT.hpp>
#include <opentxs/client/OTME_too.hpp>
#include <opentxs/core/Log.hpp>

namespace opentxs
{
CmdCanMessage::CmdCanMessage()
{
    command = "canmessage";
    args[0] = "--sender <nym>";
    args[1] = "--recipient <nym>";
    category = catOtherUsers;
    help = "Determine if prerequisites for messaging are met";
}

int32_t CmdCanMessage::runWithOptions()
{
    return run(getOption("sender"), getOption("recipient"));
}

int32_t CmdCanMessage::run(
    const std::string& sender,
    const std::string& recipient)
{
    if (sender.empty()) {
        return -1;
    }

    if (recipient.empty()) {
        return -1;
    }

    const auto response =
        OT::App().API().OTME_TOO().CanMessage(sender, recipient);

    otOut << std::to_string(static_cast<std::int8_t>(response)) << std::endl;

    return 0;
}
} // namespace opentxs
