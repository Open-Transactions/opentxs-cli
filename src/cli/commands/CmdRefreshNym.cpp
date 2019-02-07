// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdRefreshNym.hpp"

#include <opentxs/opentxs.hpp>

#include <ostream>
#include <stdint.h>
#include <string>

#define OT_METHOD "opentxs::CmdRefreshNym::"

using namespace opentxs;
using namespace std;

CmdRefreshNym::CmdRefreshNym()
{
    command = "refreshnym";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    category = catNyms;
    help = "Download mynym's latest intermediary files.";
}

CmdRefreshNym::~CmdRefreshNym() {}

int32_t CmdRefreshNym::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"));
}

int32_t CmdRefreshNym::run(string server, string mynym)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    auto task = Opentxs::Client().OTX().DownloadNymbox(
        Identifier::Factory(mynym), Identifier::Factory(server));

    const auto result = std::get<1>(task).get();
    
    const auto success = CmdBase::GetResultSuccess(result);
    if (false == success) {
        LogOutput(OT_METHOD)(__FUNCTION__)(": Failed to refresh nym").Flush();
        return -1;
    }

    return 1;
}
