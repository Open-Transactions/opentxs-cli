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

#include "CmdGetPeerRequest.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{

CmdGetPeerRequest::CmdGetPeerRequest()
{
    command = "getpeerrequest";
    args[0] = "--mynym <nym>";
    args[1] = "--request <request ID>";
    category = catOtherUsers;
    help = "Show a base64-encoded peer request";
}

std::int32_t CmdGetPeerRequest::runWithOptions()
{
    return run(getOption("mynym"), getOption("request"));
}

std::int32_t CmdGetPeerRequest::run(std::string mynym, std::string request)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    if (request.empty()) { return -1; }

    const auto text = SwigWrap::getRequest_Base64(mynym, request);

    otOut << "Peer request ID: " << request << std::endl;
    otOut << text << std::endl;

    return 1;
}
}  // namespace opentxs
