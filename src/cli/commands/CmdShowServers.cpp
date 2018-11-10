// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdShowServers.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

#define OT_METHOD "opentxs::CmdShowServers::"

using namespace opentxs;
using namespace std;

CmdShowServers::CmdShowServers()
{
    command = "showservers";
    category = catWallet;
    help = "Show the server contracts in the wallet.";
}

CmdShowServers::~CmdShowServers() {}

int32_t CmdShowServers::runWithOptions() { return run(); }

int32_t CmdShowServers::run()
{
    int32_t items = SwigWrap::GetServerCount();
    if (0 > items) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot load server list item count.")
            .Flush();
        return -1;
    }

    if (0 == items) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": The server list is empty.")
            .Flush();
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
