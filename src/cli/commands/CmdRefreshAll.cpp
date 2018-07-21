// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdRefreshAll.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{

CmdRefreshAll::CmdRefreshAll()
{
    command = "refreshall";
    category = catWallet;
    help = "Refresh all nyms in the wallet on all servers.";
}

int32_t CmdRefreshAll::runWithOptions() { return run(); }

int32_t CmdRefreshAll::run()
{
    SwigWrap::Trigger_Refresh();

    return 1;
}
}  // namespace opentxs
