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

#include "CmdGetContact.hpp"

#include "CmdBase.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/OTAPI_Wrap.hpp>
#include <opentxs/core/Log.hpp>

namespace opentxs
{
CmdGetContact::CmdGetContact()
{
    command = "getcontact";
    args[0] = "--hisnym <nymid>";
    category = catOtherUsers;
    help = "Look up the contact ID for a nym ID";
}

std::int32_t CmdGetContact::runWithOptions()
{
    return run(getOption("hisnym"));
}

std::int32_t CmdGetContact::run(const std::string& hisnym)
{
    const auto contact = OTAPI_Wrap::Nym_to_Contact(hisnym);

    if (contact.empty()) {
        otErr << "Nym " << hisnym << " is not associated with a contact."
              << std::endl;
    } else {
        otErr << contact << std::endl;
    }

    return 0;
}
} // namespace opentxs
