// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdShowBalance.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

#define OT_METHOD "opentxs::CmdShowBalance::"

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
    if (!checkAccount("myacct", myacct)) { return -1; }

    string assetType = getAccountAssetType(myacct);
    if ("" == assetType) { return -1; }

    int64_t balance = SwigWrap::GetAccountWallet_Balance(myacct);
    if (OT_ERROR_AMOUNT == balance) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot get account balance.")
            .Flush();
        return -1;
    }

    string name = SwigWrap::GetAccountWallet_Name(myacct);
    if ("" == name) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot get account name.")
            .Flush();
        return -1;
    }

    cout << "Balance: " << formatAmount(assetType, balance) << "\n";
    cout << myacct << " (" << name << ")\n\n";

    return 1;
}
