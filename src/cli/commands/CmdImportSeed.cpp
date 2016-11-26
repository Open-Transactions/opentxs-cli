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

#include "CmdImportSeed.hpp"

#include "CmdBase.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/OTAPI_Wrap.hpp>


#include <stdint.h>
#include <iostream>
#include <string>

namespace opentxs
{
CmdImportSeed::CmdImportSeed()
{
    command = "importseed";
    args[0] = "--words <word list>";
    args[1] = "[--phrase <passphrase>]";
    category = catWallet;
    help = "Add a BIP39 seed to the wallet";
}

int32_t CmdImportSeed::runWithOptions()
{
    return run(getOption("words"), getOption("phrase"));
}

int32_t CmdImportSeed::run(const std::string& words, const std::string& phrase)
{
    if (words.empty()) {

        return -1;
    }

    const std::string fingerprint =
        OTAPI_Wrap::Wallet_ImportSeed(words, phrase);
    const bool empty = fingerprint.empty();

    if (!empty) {
        std::cout << fingerprint << std::endl;
    }

    return empty ? -1 : 0;
}
} // namespace opentxs
