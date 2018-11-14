// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdVerifySignature.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

#define OT_METHOD "opentxs::CmdVerifySignature::"

using namespace opentxs;
using namespace std;

CmdVerifySignature::CmdVerifySignature()
{
    command = "verifysignature";
    args[0] = "--hisnym <nym>";
    category = catAdmin;
    help = "Verify hisnym's signature on a pasted contract.";
}

CmdVerifySignature::~CmdVerifySignature() {}

int32_t CmdVerifySignature::runWithOptions()
{
    return run(getOption("hisnym"));
}

int32_t CmdVerifySignature::run(string hisnym)
{
    if (!checkNym("hisnym", hisnym)) { return -1; }

    string contract = inputText("the contract to verify");
    if ("" == contract) { return -1; }

    if (!SwigWrap::VerifySignature(hisnym, contract)) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot verify signature.")
            .Flush();
        return -1;
    }

    cout << "Signature was verified!\n";

    return 1;
}
