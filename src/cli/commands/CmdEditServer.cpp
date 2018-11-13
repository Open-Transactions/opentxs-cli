// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdEditServer.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

#define OT_METHOD "opentxs::CmdEditServer::"

using namespace opentxs;
using namespace std;

CmdEditServer::CmdEditServer()
{
    command = "editserver";
    args[0] = "--server <server>";
    args[1] = "--label <label>";
    category = catWallet;
    help = "Edit server's label, as it appears in your wallet.";
}

CmdEditServer::~CmdEditServer() {}

int32_t CmdEditServer::runWithOptions()
{
    return run(getOption("server"), getOption("label"));
}

int32_t CmdEditServer::run(string server, string label)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkMandatory("label", label)) { return -1; }

    if (!SwigWrap::SetServer_Name(server, label)) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot set server label.")
            .Flush();
        return -1;
    }

    return 1;
}
