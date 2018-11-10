// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdClearExpired.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

#define OT_METHOD "opentxs::CmdClearExpired::"

using namespace opentxs;
using namespace std;

CmdClearExpired::CmdClearExpired()
{
    command = "clearexpired";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    category = catMisc;
    help = "Clear all mynym's expired records.";
}

CmdClearExpired::~CmdClearExpired() {}

int32_t CmdClearExpired::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"));
}

int32_t CmdClearExpired::run(string server, string mynym)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    if (!SwigWrap::ClearExpired(server, mynym, 0, true)) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            " : Error: cannot clear expired records.")
            .Flush();
        return -1;
    }

    return 1;
}
