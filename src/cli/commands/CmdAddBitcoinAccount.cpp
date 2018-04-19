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

#include "CmdAddBitcoinAccount.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{
CmdAddBitcoinAccount::CmdAddBitcoinAccount()
{
    command = "addbitcoinaccount";
    args[0] = "--mynym <nym>";
    category = catBlockchain;
    help = "Instantiate a BIP-44 account";
}

std::int32_t CmdAddBitcoinAccount::runWithOptions()
{
    return run(getOption("mynym"));
}

std::int32_t CmdAddBitcoinAccount::run(std::string mynym)
{
    if (!checkNym("mynym", mynym)) {

        return -1;
    }

    const auto account = OT::App().Blockchain().NewAccount(
        Identifier(mynym), BlockchainAccountType::BIP44, proto::CITEMTYPE_BTC);
    otOut << "BIP-44 account ID: " << String(account) << std::endl;

    return 0;
}
}  // namespace opentxs
