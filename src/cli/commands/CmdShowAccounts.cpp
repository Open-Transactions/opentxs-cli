// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdShowAccounts.hpp"

#include <opentxs/opentxs.hpp>

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

CmdShowAccounts::~CmdShowAccounts() {}

int32_t CmdShowAccounts::runWithOptions() { return run(); }

int32_t CmdShowAccounts::run()
{
    const auto& storage = Opentxs::Client().Storage();
    cout << " ** ACCOUNTS:\n";
    dashLine();

    for (const auto& it : storage.AccountList()) {
        const auto& myacct = std::get<0>(it);

        if ("" == myacct) {
            otOut << "Error: cannot load account.\n";
            return -1;
        }

        string accountData = stat_asset_account(myacct);

        if ("" == accountData) {
            cout << "Error : cannot load account data.\n";
            return -1;
        }

        cout << accountData << "\n";
        dashLine();
    }

    return 1;
}
