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

#include "CmdExportNym.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdExportNym::CmdExportNym()
{
    command = "exportnym";
    args[0] = "--mynym <nym>";
    category = catWallet;
    help = "Export myym as a single importable file.";
}

CmdExportNym::~CmdExportNym() {}

int32_t CmdExportNym::runWithOptions() { return run(getOption("mynym")); }

int32_t CmdExportNym::run(string mynym)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    string exportedNym = SwigWrap::Wallet_ExportNym(mynym);
    if ("" == exportedNym) {
        otOut << "Error: cannor export nym.\n";
        return -1;
    }

    cout << exportedNym << "\n";
    return 1;
}
