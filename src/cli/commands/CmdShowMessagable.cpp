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

#include "CmdShowMessagable.hpp"


#include <opentxs/api/Native.hpp>
#include <opentxs/api/UI.hpp>
#include <opentxs/core/Identifier.hpp>
#include <opentxs/core/Log.hpp>
#include <opentxs/ui/ContactListItem.hpp>
#include <opentxs/ui/MessagableList.hpp>
#include <opentxs/OT.hpp>

namespace opentxs
{

CmdShowMessagable::CmdShowMessagable()
{
    command = "showmessagable";
    args[0] = "--mynym <nym>";
    category = catOtherUsers;
    help = "Show the list of messabable contacts for a nym in the wallet.";
}

std::int32_t CmdShowMessagable::runWithOptions()
{
    return run(getOption("mynym"));
}

std::int32_t CmdShowMessagable::run(std::string mynym)
{
    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    const Identifier nymID{mynym};
    auto& list = OT::App().UI().MessagableList(nymID);
    otOut << "Contacts:\n";
    dashLine();
    auto& line = list.First();
    auto last = line.Last();

    if (false == line.Valid()) {

        return 1;
    }

    otOut << " " << line.Section() << " " << line.DisplayName() << " ("
          << line.ContactID() << ")\n";

    while (false == last) {
        auto& line = list.Next();
        last = line.Last();

        if (false == line.Valid()) {

            return 1;
        }
        otOut << " " << line.Section() << "  " << line.DisplayName() << " ("
              << line.ContactID() << ")\n";
    }

    otOut << std::endl;

    return 1;
}
} // namespace opentxs
