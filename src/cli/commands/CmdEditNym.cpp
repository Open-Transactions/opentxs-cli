// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdEditNym.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

#define OT_METHOD "opentxs::CmdEditNym::"

using namespace opentxs;
using namespace std;

CmdEditNym::CmdEditNym()
{
    command = "editnym";
    args[0] = "--mynym <nym>";
    args[1] = "--label <label>";
    category = catWallet;
    help = "Edit mynym's label, as it appears in your wallet.";
}

CmdEditNym::~CmdEditNym() {}

int32_t CmdEditNym::runWithOptions()
{
    return run(getOption("mynym"), getOption("label"));
}

int32_t CmdEditNym::run(string mynym, string label)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    if (!checkMandatory("label", label)) { return -1; }

    if (!SwigWrap::SetNym_Alias(mynym, mynym, label)) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot set nym label.")
            .Flush();
        return -1;
    }

    return 1;
}
