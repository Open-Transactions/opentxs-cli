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

#include "CmdShowWallet.hpp"

#include "CmdBase.hpp"
#include "CmdShowAccounts.hpp"
#include "CmdShowAssets.hpp"
#include "CmdShowNyms.hpp"
#include "CmdShowServers.hpp"

#include <stdint.h>
#include <iostream>

using namespace opentxs;
using namespace std;

CmdShowWallet::CmdShowWallet()
{
    command = "showwallet";
    category = catWallet;
    help = "Show wallet contents.";
}

CmdShowWallet::~CmdShowWallet()
{
}

int32_t CmdShowWallet::runWithOptions()
{
    return run();
}

int32_t CmdShowWallet::run()
{
    CmdShowServers showServers;
    bool success = 0 <= showServers.run();

    cout << "\n";

    CmdShowNyms showNyms;
    success |= 0 <= showNyms.run();

    cout << "\n";

    CmdShowAssets showAssets;
    success |= 0 <= showAssets.run();

    cout << "\n";

    CmdShowAccounts showAccounts;
    success |= 0 <= showAccounts.run();

    return success ? 1 : -1;
}
