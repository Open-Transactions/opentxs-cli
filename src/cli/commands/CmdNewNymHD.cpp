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

#include "CmdNewNymHD.hpp"

#include "CmdBase.hpp"

#include <opentxs/client/SwigWrap.hpp>
#include <opentxs/core/Log.hpp>
#include <opentxs/Types.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdNewNymHD::CmdNewNymHD()
{
    command = "newnymhd";
    args[0] = "--label <label>";
    args[1] = "[--source <seed fingerprint>]";
    args[2] = "[--index <HD derivation path>]";
    category = catNyms;
    help = "create a new nym using HD key derivation.";
}

int32_t CmdNewNymHD::runWithOptions()
{
    return run(getOption("label"), getOption("source"), getOption("index"));
}

int32_t CmdNewNymHD::run(string label, string source, string path)
{
    if (!checkMandatory("label", label)) {
        return -1;
    }

    std::uint32_t nym = 0;

    if (!path.empty()) {
        try {
            nym = stoul(path);
        }
        catch (std::invalid_argument) { nym = 0; }
        catch (std::out_of_range) { nym = 0; }

        const std::uint32_t hardened =
            static_cast<std::uint32_t>(opentxs::Bip32Child::HARDENED);

        if (hardened <= nym) {
            nym = nym ^ hardened;
        }
    }


    std::string mynym = SwigWrap::CreateIndividualNym(label, source, nym);

    if ("" == mynym) {
        otOut << "Error: cannot create new nym.\n";
        return -1;
    }

    cout << "New nym: " << mynym << "\n";

    return 1;
}
