// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdDecode.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdDecode::CmdDecode()
{
    command = "decode";
    category = catAdmin;
    help = "Decode OT-armored input text.";
}

CmdDecode::~CmdDecode() {}

int32_t CmdDecode::runWithOptions() { return run(); }

int32_t CmdDecode::run()
{
    string input = inputText("the OT-armored text to be decoded");
    if ("" == input) { return -1; }

    string output = SwigWrap::Decode(input, true);
    if ("" == output) {
        otOut << "Error: cannot decode input.\n";
        return -1;
    }

    dashLine();
    otOut << "Decoded:\n\n";
    cout << output << "\n";

    return 1;
}
