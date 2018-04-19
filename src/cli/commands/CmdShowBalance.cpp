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

#include "CmdShowBalance.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdShowBalance::CmdShowBalance()
{
    command = "showbalance";
    args[0] = "--myacct <account>";
    category = catAccounts;
    help = "Show myacct's balance.";
}

CmdShowBalance::~CmdShowBalance() {}

int32_t CmdShowBalance::runWithOptions() { return run(getOption("myacct")); }

int32_t CmdShowBalance::run(string myacct)
{
    if (!checkAccount("myacct", myacct)) {
        return -1;
    }

    string assetType = getAccountAssetType(myacct);
    if ("" == assetType) {
        return -1;
    }

    int64_t balance = SwigWrap::GetAccountWallet_Balance(myacct);
    if (OT_ERROR_AMOUNT == balance) {
        otOut << "Error: cannot get account balance.\n";
        return -1;
    }

    string name = SwigWrap::GetAccountWallet_Name(myacct);
    if ("" == name) {
        otOut << "Error: cannot get account name.\n";
        return -1;
    }

    cout << "Balance: " << formatAmount(assetType, balance) << "\n";
    cout << myacct << " (" << name << ")\n\n";

    return 1;
}
