// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdAddAsset.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdAddAsset::CmdAddAsset()
{
    command = "addasset";
    category = catWallet;
    help = "Import an existing asset contract into your wallet.";
}

CmdAddAsset::~CmdAddAsset() {}

int32_t CmdAddAsset::runWithOptions() { return run(); }

int32_t CmdAddAsset::run()
{
    string contract = inputText("an asset contract");
    if ("" == contract) { return -1; }

    if (SwigWrap::AddUnitDefinition(contract).empty()) {
        otOut << "Error: cannot add asset contract.\n";
        return -1;
    }

    return 1;
}
