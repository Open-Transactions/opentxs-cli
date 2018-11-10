// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdChangePw.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <ostream>

#define OT_METHOD "opentxs::CmdChangePw::"

using namespace opentxs;
using namespace std;

CmdChangePw::CmdChangePw()
{
    command = "changepw";
    category = catWallet;
    help = "Change the master passphrase for the wallet.";
}

CmdChangePw::~CmdChangePw() {}

int32_t CmdChangePw::runWithOptions() { return run(); }

int32_t CmdChangePw::run()
{
    if (!SwigWrap::Wallet_ChangePassphrase()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot change passphrase.")
            .Flush();
        return -1;
    }

    return 1;
}
