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

#include "CmdPairStatus.hpp"

#include <opentxs/client/SwigWrap.hpp>

namespace opentxs {

CmdPairStatus::CmdPairStatus()
{
    command = "pairstatus";
    args[0] = "--mynym <nym ID>";
    args[1] = "--issuer <issuer nym ID>";
    category = catMisc;
    help = "Pairing status";
}

std::int32_t CmdPairStatus::runWithOptions()
{
    return run(getOption("mynym"), getOption("issuer"));
}

std::int32_t CmdPairStatus::run(
    std::string localNym,
    const std::string& issuerNym)
{
    if (!checkNym("mynym", localNym)) {

        return -1;
    }

    std::cout << SwigWrap::Pair_Status(localNym, issuerNym);

    return 1;
}
} // namespace opentxs
