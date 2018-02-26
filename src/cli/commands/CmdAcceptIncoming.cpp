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

#include "CmdAcceptIncoming.hpp"

#include <opentxs/api/client/Sync.hpp>
#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/client/OT_API.hpp>
#include <opentxs/client/OTWallet.hpp>
#include <opentxs/core/Account.hpp>
#include <opentxs/OT.hpp>

#include <cstdint>
#include <string>

namespace opentxs
{
CmdAcceptIncoming::CmdAcceptIncoming()
{
    command = "acceptincoming";
    args[0] = "--myacct <account>";
    category = catAccounts;
    help = "Accept all incoming payments in myacct's inbox.";
}

std::int32_t CmdAcceptIncoming::runWithOptions()
{
    return run(getOption("myacct"));
}

std::int32_t CmdAcceptIncoming::run(std::string myacct)
{
    if (!checkAccount("myacct", myacct)) {
        return -1;
    }

    const auto& otapi =  OT::App().API().OTAPI();
    auto wallet = otapi.GetWallet();

    OT_ASSERT(nullptr != wallet);

    const Identifier accountID(myacct);
    const auto account = otapi.GetAccount(accountID);

    OT_ASSERT(account);

    const auto& nymID = account->GetNymID();
    const auto& serverID = account->GetRealNotaryID();

    const auto output = OT::App().API().Sync().AcceptIncoming(
        nymID, accountID, serverID);

    if (!output) {
        otErr << "Failed to accept incoming payments." << std::endl;

        return -1;
    }

    return 0;
}
}  // namespace opentxs
