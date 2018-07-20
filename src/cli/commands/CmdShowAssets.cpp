// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdShowAssets.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdShowAssets::CmdShowAssets()
{
    command = "showassets";
    category = catWallet;
    help = "Show the currency contracts in the wallet.";
}

CmdShowAssets::~CmdShowAssets() {}

int32_t CmdShowAssets::runWithOptions() { return run(); }

int32_t CmdShowAssets::run()
{
    int32_t items = SwigWrap::GetAssetTypeCount();
    if (0 > items) {
        otOut << "Error: cannot load instrument definition list count.\n";
        return -1;
    }

    if (0 == items) {
        otOut << "The instrument definition list is empty.\n";
        return 0;
    }

    cout << " ** ASSET TYPES:\n";
    dashLine();

    for (int32_t i = 0; i < items; i++) {
        string assetType = SwigWrap::GetAssetType_ID(i);
        string name = SwigWrap::GetAssetType_Name(assetType);
        cout << i << ": " << assetType << "  -  " << name << "\n";
    }

    return 1;
}
