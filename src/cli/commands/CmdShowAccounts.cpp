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

#include "CmdShowAccounts.hpp"

#include "CmdBase.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/api/Api.hpp>
#include <opentxs/api/OT.hpp>
#include <opentxs/client/MadeEasy.hpp>
#include <opentxs/client/SwigWrap.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdShowAccounts::CmdShowAccounts()
{
    command = "showaccounts";
    category = catWallet;
    help = "Show the asset accounts in the wallet.";
}

CmdShowAccounts::~CmdShowAccounts()
{
}

int32_t CmdShowAccounts::runWithOptions()
{
    return run();
}

int32_t CmdShowAccounts::run()
{
    int32_t items = SwigWrap::GetAccountCount();
    if (0 > items) {
        otOut << "Error: cannot load account list count.\n";
        return -1;
    }

    if (0 == items) {
        otOut << "The account list is empty.\n";
        return 0;
    }

    cout << " ** ACCOUNTS:\n";
    dashLine();

    for (int32_t i = 0; i < items; i++) {
        string myacct = SwigWrap::GetAccountWallet_ID(i);
        if ("" == myacct) {
            otOut << "Error: cannot load account.\n";
            return -1;
        }

        string accountData = OT::App().API().ME().stat_asset_account(myacct);
        if ("" == accountData) {
            cout << "Error : cannot load account data.\n";
            return -1;
        }

        cout << accountData << "\n";
        dashLine();
    }

    return 1;
}
