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

#include "CmdMergeContact.hpp"


#include <opentxs/api/ContactManager.hpp>
#include <opentxs/api/OT.hpp>
#include <opentxs/contact/Contact.hpp>
#include <opentxs/contact/ContactData.hpp>
#include <opentxs/core/Log.hpp>

namespace opentxs
{
CmdMergeContact::CmdMergeContact()
{
    command = "mergecontact";
    args[0] = "--id <id>";
    args[1] = "--into <id>";
    category = catOtherUsers;
    help = "Merge two contacts";
}

std::int32_t CmdMergeContact::runWithOptions()
{
    return run(getOption("into"), getOption("id"));
}

std::int32_t CmdMergeContact::run(
    const std::string& parent,
    const std::string& child)
{
    auto contact =
        OT::App().Contact().Merge(Identifier(parent), Identifier(child));

    if (false == bool(contact)) {

        return -1;
    }

    otOut << contact->Print();

    return 1;
}
} // namespace opentxs
