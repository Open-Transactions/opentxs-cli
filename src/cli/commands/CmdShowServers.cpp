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

#include "CmdShowServers.hpp"

#include "CmdBase.hpp"

#include <opentxs/client/SwigWrap.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdShowServers::CmdShowServers()
{
    command = "showservers";
    category = catWallet;
    help = "Show the server contracts in the wallet.";
}

CmdShowServers::~CmdShowServers()
{
}

int32_t CmdShowServers::runWithOptions()
{
    return run();
}

int32_t CmdShowServers::run()
{
    int32_t items = SwigWrap::GetServerCount();
    if (0 > items) {
        otOut << "Error: cannot load server list item count.\n";
        return -1;
    }

    if (0 == items) {
        otOut << "The server list is empty.\n";
        return 0;
    }

    cout << " ** SERVERS:\n";
    dashLine();

    for (int32_t i = 0; i < items; i++) {
        string server = SwigWrap::GetServer_ID(i);
        string name = SwigWrap::GetServer_Name(server);
        cout << i << ": " << server << "  -  " << name << "\n";
    }

    return 1;
}
