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

#include "CmdPairNode.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/OTAPI_Wrap.hpp>

namespace opentxs {

CmdPairNode::CmdPairNode()
{
    command = "pairnode";
    args[0] = "--mynym <nym>";
    args[1] = "--hisnym <bridge nym>";
    args[2] = "--password <server password>";
    category = catMisc;
    help = "Pair with a Stash Node";
}

std::int32_t CmdPairNode::runWithOptions()
{
    return run(
        getOption("mynym"),
        getOption("hisnym"),
        getOption("password"));
}

std::int32_t CmdPairNode::run(
    std::string mynym,
    std::string hisnym,
    std::string password)
{
    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    auto result = OTAPI_Wrap::Pair_Node(mynym, hisnym, password);

    if (result) {
        return 1;
    }

    return 0;
}
} // namespace opentxs
