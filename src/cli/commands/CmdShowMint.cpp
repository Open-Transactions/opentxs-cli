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

#include "CmdShowMint.hpp"

#include "CmdBase.hpp"

#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/client/OT_ME.hpp>
#include <opentxs/core/Log.hpp>
#include <opentxs/OT.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdShowMint::CmdShowMint()
{
    command = "showmint";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--mypurse <purse>";
    category = catAdmin;
    help = "Show mint file for specific instrument definition id. Download if "
           "necessary.";
}

CmdShowMint::~CmdShowMint() {}

int32_t CmdShowMint::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("mypurse"));
}

int32_t CmdShowMint::run(string server, string mynym, string mypurse)
{
#if OT_CASH
    if (!checkServer("server", server)) {
        return -1;
    }

    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    if (!checkPurse("mypurse", mypurse)) {
        return -1;
    }

    string mint =
        OT::App().API().OTME().load_or_retrieve_mint(server, mynym, mypurse);
    if ("" == mint) {
        otOut << "Error: cannot load mint.\n";
        return -1;
    }

    cout << mint << "\n";

    return 1;
#else
    return -1;
#endif  // OT_CASH
}
