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

#include "CmdGetPeerReplies.hpp"

#include "CmdBase.hpp"
#include <opentxs/core/Version.hpp>
#include <opentxs/client/OTAPI_Wrap.hpp>
#include <opentxs/core/Log.hpp>

namespace opentxs {

CmdGetPeerReplies::CmdGetPeerReplies()
{
    command = "getpeerreplies";
    args[0] = "--mynym <nym>";
    category = catOtherUsers;
    help = "Get a list of incoming peer reply IDs";
}

CmdGetPeerReplies::~CmdGetPeerReplies()
{
}

std::int32_t CmdGetPeerReplies::runWithOptions()
{
    return run(getOption("mynym"));
}

std::int32_t CmdGetPeerReplies::run(std::string mynym)
{
    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    auto output = OTAPI_Wrap::getIncomingReplies(mynym);

    otOut << output << std::endl;

    return 1;
}
} // namespace opentxs
