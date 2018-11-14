// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdEncode.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

#define OT_METHOD "opentxs::CmdEncode"

using namespace opentxs;
using namespace std;

CmdEncode::CmdEncode()
{
    command = "encode";
    category = catAdmin;
    help = "Encode plaintext input to OT-armored text.";
}

CmdEncode::~CmdEncode() {}

int32_t CmdEncode::runWithOptions() { return run(); }

int32_t CmdEncode::run()
{
    string input = inputText("the plaintext to be encoded");
    if ("" == input) { return -1; }

    string output = SwigWrap::Encode(input, true);
    if ("" == output) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot encode input.")
            .Flush();
        return -1;
    }

    dashLine();
    LogNormal(OT_METHOD)(__FUNCTION__)(": Encoded:").Flush();
    cout << output << "\n";

    return 1;
}
