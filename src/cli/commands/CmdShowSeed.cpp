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

#include "CmdShowSeed.hpp"

#include "CmdBase.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/SwigWrap.hpp>


#include <stdint.h>
#include <iostream>
#include <string>

namespace opentxs
{
CmdShowSeed::CmdShowSeed()
{
    command = "showseed";
    category = catWallet;
    help = "Show the wallet BIP32 seed as a hex string.";
}

CmdShowSeed::~CmdShowSeed()
{
}

int32_t CmdShowSeed::runWithOptions()
{
    return run();
}

int32_t CmdShowSeed::run()
{
    const std::string seed = SwigWrap::Wallet_GetSeed();
    const bool empty = 1 > seed.size();
    std::cout << seed << std::endl;

    return empty ? -1 : 0;
}
} // namespace opentxs
