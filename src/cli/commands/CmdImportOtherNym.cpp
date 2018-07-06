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

#include "CmdImportOtherNym.hpp"

#include <opentxs/opentxs.hpp>

#include <iostream>
#include <string>

namespace opentxs
{
CmdImportOtherNym::CmdImportOtherNym()
{
    command = "importothernym";
    category = catWallet;
    help = "Import a pasted nym.";
}

std::int32_t CmdImportOtherNym::runWithOptions() { return run(); }

std::int32_t CmdImportOtherNym::run()
{
    std::string input = inputText("an exported nym");

    if ("" == input) { return -1; }

    std::string outNym = SwigWrap::Import_Nym(input);

    if ("" == outNym) {
        otOut << "Error: cannot import Nym.\n";

        return -1;
    }

    std::cout << "Imported nym: " << outNym << ".\n";

    return 1;
}
}  // namespace opentxs
