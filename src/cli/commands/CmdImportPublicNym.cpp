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

#include "CmdImportPublicNym.hpp"

#include <opentxs/opentxs.hpp>

#include <string>

namespace opentxs
{

CmdImportPublicNym::CmdImportPublicNym()
{
    command = "importpublicnym";
    category = catWallet;
    help = "Import a public nym.";
}

std::int32_t CmdImportPublicNym::runWithOptions() { return run(); }

std::int32_t CmdImportPublicNym::run()
{
    const auto input = inputText("An armored public nym");

    if (input.empty()) {

        return -1;
    }

    const auto nymID = SwigWrap::Import_Nym(input);

    if (nymID.empty()) {
        otOut << "Error: cannot import Nym.\n";

        return -1;
    }

    otOut << "Imported nym: " << nymID << ".\n";

    return 1;
}
}  // namespace opentxs
