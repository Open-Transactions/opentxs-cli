// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdAddServer.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

#define OT_METHOD "opentxs::CmdAddServer::"

using namespace opentxs;
using namespace std;

CmdAddServer::CmdAddServer()
{
    command = "addserver";
    category = catWallet;
    help = "Import an existing server contract into your wallet.";
}

CmdAddServer::~CmdAddServer() {}

int32_t CmdAddServer::runWithOptions() { return run(); }

int32_t CmdAddServer::run()
{
    string contract = inputText("a server contract");
    if ("" == contract) { return -1; }

    if ("" == SwigWrap::AddServerContract(contract)) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot add server contract.")
            .Flush();
        return -1;
    }

    return 1;
}
