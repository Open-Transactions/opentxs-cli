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

#include "CmdShowPayable.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{

CmdShowPayable::CmdShowPayable()
{
    command = "showpayable";
    args[0] = "--mynym <nym>";
    args[1] = "--currency <id>";
    category = catOtherUsers;
    help = "Show the list of payable contacts for a currency.";
}

std::int32_t CmdShowPayable::runWithOptions()
{
    return run(getOption("mynym"), getOption("currency"));
}

std::int32_t CmdShowPayable::run(std::string mynym, std::string currency)
{
    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    if (!checkMandatory("currency", currency)) {
        return -1;
    }

    auto currencyType = proto::ContactItemType(std::stoi(currency));

    const Identifier nymID{mynym};
    auto& list = OT::App().UI().PayableList(nymID, currencyType);
    otOut << "Contacts:\n";
    dashLine();
    auto line = list.First();
    auto last = line->Last();

    if (false == line->Valid()) {

        return 1;
    }

    otOut << " " << line->Section() << " " << line->DisplayName() << " ("
          << line->ContactID() << ")\n";

    while (false == last) {
        line = list.Next();
        last = line->Last();

        if (false == line->Valid()) {

            return 1;
        }
        otOut << " " << line->Section() << "  " << line->DisplayName() << " ("
              << line->ContactID() << ")\n";
    }

    otOut << std::endl;

    return 1;
}
}  // namespace opentxs
