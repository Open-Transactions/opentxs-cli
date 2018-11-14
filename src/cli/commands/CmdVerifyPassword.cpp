// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdVerifyPassword.hpp"

#include <opentxs/opentxs.hpp>

#define OT_METHOD "opentxs::CmdVerifyPassword::"

namespace opentxs
{

CmdVerifyPassword::CmdVerifyPassword()
{
    command = "verifypassword";
    category = catWallet;
    help = "Prompt for wallet passphrase entry.";
}

std::int32_t CmdVerifyPassword::runWithOptions() { return run(); }

std::int32_t CmdVerifyPassword::run()
{
    if (false == SwigWrap::Wallet_CheckPassword()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: invalid passphrase.")
            .Flush();

        return -1;
    } else {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Password confirmed.").Flush();

        return 1;
    }
}
}  // namespace opentxs
