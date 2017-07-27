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

#include "CmdShowNym.hpp"

#include <opentxs/core/Version.hpp>

#include <opentxs/api/OT.hpp>
#include <opentxs/api/Wallet.hpp>
#include <opentxs/client/OTAPI_Wrap.hpp>
#include <opentxs/core/Identifier.hpp>
#include <opentxs/core/Proto.hpp>

#include <iostream>

namespace opentxs
{
CmdShowNym::CmdShowNym()
{
    command = "shownym";
    args[0] = "--mynym <nym>";
    category = catNyms;
    help = "Show mynym's statistics.";
}

std::int32_t CmdShowNym::runWithOptions()
{
    return run(getOption("mynym"));
}

std::int32_t CmdShowNym::run(std::string mynym)
{
    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    std::string nymStats = OTAPI_Wrap::GetNym_Stats(mynym);

    if ("" == nymStats) {
        nymStats = "This nym is not located in the local wallet.";
    }

    std::string claims = OTAPI_Wrap::DumpContactData(mynym);
    std::cout << nymStats << std::endl << claims;
    auto nym = OT::App().Contract().Nym(Identifier(mynym));

    if (nym) {
        const auto armored =
            proto::ProtoAsArmored(nym->asPublicNym(), "PUBLIC NYM");
        std::cout << armored << std::endl;
    }

    return 1;
}
} // namespace opentxs
