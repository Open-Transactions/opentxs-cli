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

#include "CmdShowContacts.hpp"


#include <opentxs/api/Api.hpp>
#include <opentxs/api/ContactManager.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/api/OT.hpp>
#include <opentxs/contact/Contact.hpp>
#include <opentxs/contact/ContactData.hpp>
#include <opentxs/core/Identifier.hpp>
#include <opentxs/core/Log.hpp>
#include <opentxs/core/String.hpp>
#include <opentxs/storage/Storage.hpp>

namespace opentxs
{

CmdShowContacts::CmdShowContacts()
{
    command = "showcontacts";
    category = catOtherUsers;
    help = "Show the contacts in the wallet.";
}

std::int32_t CmdShowContacts::runWithOptions()
{
    return run();
}

std::int32_t CmdShowContacts::run()
{
    auto& ot = OT::App();
    auto& storage = ot.DB();
    const auto contactList = storage.ContactList();
    otOut << "Contacts:\n";
    dashLine();

    for (const auto& it : contactList) {
        const auto& contactID = it.first;
        auto contact = ot.Contact().Contact(Identifier(contactID));

        OT_ASSERT(contact);

        otOut << " * " << contactID << " (" << contact->Label() << ")\n";
    }

    otOut << std::endl;

    return 1;
}
} // namespace opentxs
