// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdImportNym.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

#define OT_METHOD "opentxs::CmdImportNym::"

using namespace opentxs;
using namespace std;

CmdImportNym::CmdImportNym()
{
    command = "importnym";
    category = catWallet;
    help = "Import a pasted nym.";
}

CmdImportNym::~CmdImportNym() {}

int32_t CmdImportNym::runWithOptions() { return run(); }

int32_t CmdImportNym::run()
{
    string input = inputText("an exported nym");
    if ("" == input) { return -1; }

    string outNym = SwigWrap::Wallet_ImportNym(input);
    if ("" == outNym) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot import Nym.")
            .Flush();
        return -1;
    }

    cout << "Imported nym: " << outNym << ".\n";

    return 1;
}
