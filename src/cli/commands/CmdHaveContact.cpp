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

#include "CmdHaveContact.hpp"


#include <opentxs/api/Api.hpp>
#include <opentxs/api/ContactManager.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/OT.hpp>
#include <opentxs/core/Identifier.hpp>
#include <opentxs/core/Log.hpp>
#include <opentxs/core/String.hpp>

namespace opentxs
{
CmdHaveContact::CmdHaveContact()
{
    command = "havecontact";
    args[0] = "--contact <nym or payment code>";
    category = catOtherUsers;
    help = "Determine if a contact exists";
}

std::int32_t CmdHaveContact::runWithOptions()
{
    return run(getOption("contact"));
}

std::int32_t CmdHaveContact::run(const std::string& id)
{
    if (id.empty()) {
        return -1;
    }

    const auto contact =
        OT::App().Contact().Contact(Identifier(String(id.c_str())));

    if (contact) {
        otOut << "true" << std::endl;
    } else {
        otOut << "false" << std::endl;
    }

    return 0;
}
} // namespace opentxs
