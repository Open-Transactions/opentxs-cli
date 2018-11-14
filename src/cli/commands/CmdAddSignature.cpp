// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdAddSignature.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

#define OT_METHOD "opentxs::CmdAddSignature::"

using namespace opentxs;
using namespace std;

CmdAddSignature::CmdAddSignature()
{
    command = "addsignature";
    args[0] = "--mynym <nym>";
    category = catAdmin;
    help = "Add a signature to a contract without erasing others.";
}

CmdAddSignature::~CmdAddSignature() {}

int32_t CmdAddSignature::runWithOptions() { return run(getOption("mynym")); }

// Addsignature leaves all signatures in place and appends the new one
// Signcontract erases all signatures and replaces them with the new one
int32_t CmdAddSignature::run(string mynym)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    string contract = inputText("a contract to sign");
    if ("" == contract) { return -1; }

    string output = SwigWrap::AddSignature(mynym, contract);
    if ("" == output) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot add signature.")
            .Flush();
        return -1;
    }

    dashLine();
    cout << "Signed:\n" << output << "\n";

    return 1;
}
