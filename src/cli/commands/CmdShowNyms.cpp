// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdShowNyms.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

#define OT_METHOD "opentxs::CmdShowNyms::"

using namespace opentxs;
using namespace std;

CmdShowNyms::CmdShowNyms()
{
    command = "shownyms";
    category = catWallet;
    help = "Show the nyms in the wallet.";
}

CmdShowNyms::~CmdShowNyms() {}

int32_t CmdShowNyms::runWithOptions() { return run(); }

int32_t CmdShowNyms::run()
{
    int32_t items = SwigWrap::GetNymCount();
    if (0 > items) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot load nym list item count.")
            .Flush();
        return -1;
    }

    if (0 == items) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": The nym list is empty.").Flush();
        return 0;
    }

    cout << " ** NYMS:\n";
    dashLine();

    for (int32_t i = 0; i < items; i++) {
        string mynym = SwigWrap::GetNym_ID(i);
        string name = SwigWrap::GetNym_Name(mynym);
        cout << i << ": " << mynym << " -  " << name << "\n";
    }

    return 1;
}
