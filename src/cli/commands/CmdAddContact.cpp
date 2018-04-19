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

#include "CmdAddContact.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{
CmdAddContact::CmdAddContact()
{
    command = "addcontact";
    args[0] = "--hisnym <nym>";
    args[1] = "--label [<label>]";
    category = catOtherUsers;
    help = "Add a new contact based on Nym ID";
}

std::int32_t CmdAddContact::runWithOptions()
{
    return run(getOption("hisnym"), getOption("label"));
}

std::int32_t CmdAddContact::run(
    const std::string& hisnym,
    const std::string& label)
{
    if (hisnym.empty()) {
        return -1;
    }

    Identifier nymID(String(hisnym.c_str()));
    auto code = PaymentCode::Factory(hisnym);

    if (nymID.empty()) {
        otErr << "Provided ID was not a nymID." << std::endl;
    } else {
        otErr << "Provided ID was a nymID." << std::endl;
    }

    if (false == code->VerifyInternally()) {
        otErr << "Provided ID was not a payment code." << std::endl;
    } else {
        otErr << "Provided ID was a payment code." << std::endl;
    }

    if (nymID.empty() && code->VerifyInternally()) {
        nymID = code->ID();
        otErr << "Derived nymID: " << String(nymID) << std::endl;
    }

    const auto response = OT::App().Contact().NewContact(label, nymID, code);

    if (response) {
        return 0;
    }

    return -1;
}
}  // namespace opentxs
