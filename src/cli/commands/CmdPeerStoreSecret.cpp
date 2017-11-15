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

#include "CmdPeerStoreSecret.hpp"

#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/OT.hpp>
#include <opentxs/client/OT_ME.hpp>

namespace opentxs {

CmdPeerStoreSecret::CmdPeerStoreSecret()
{
    command = "peerstoresecret";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--hisnym <recipient>";
    category = catMisc;
    help = "Request a nym to store a BIP-39 seed on behalf of the requestor";
}

CmdPeerStoreSecret::~CmdPeerStoreSecret()
{
}

std::int32_t CmdPeerStoreSecret::runWithOptions()
{
    return run(
        getOption("server"),
        getOption("mynym"),
        getOption("hisnym"));
}

std::int32_t CmdPeerStoreSecret::run(
    std::string server,
    std::string mynym,
    std::string hisnym)
{
    if (!checkServer("server", server)) {
        return -1;
    }

    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    if (!checkNym("hisnym", hisnym)) {
        return -1;
    }

    const std::string primary = inputText("Word list");

    if (0 == primary.size()) {

        return -1;
    }

    const std::string secondary = inputText("Passphrase");



    const std::string response = OT::App().API().OTME().store_secret(
        server, mynym, hisnym, 1, primary, secondary);

    return processResponse(response, "peer store secret");
}
} // namespace opentxs
