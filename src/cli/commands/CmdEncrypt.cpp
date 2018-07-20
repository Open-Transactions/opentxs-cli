// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdEncrypt.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdEncrypt::CmdEncrypt()
{
    command = "encrypt";
    args[0] = "--hisnym <nym>";
    category = catAdmin;
    help = "Encrypt plaintext input using hisnym's public key.";
}

CmdEncrypt::~CmdEncrypt() {}

int32_t CmdEncrypt::runWithOptions() { return run(getOption("hisnym")); }

int32_t CmdEncrypt::run(string hisnym)
{
    if (!checkNym("hisnym", hisnym)) { return -1; }

    string input = inputText("the plaintext to be encrypted");
    if ("" == input) { return -1; }

    string output = SwigWrap::Encrypt(hisnym, input);
    if ("" == output) {
        otOut << "Error: cannot encrypt input.\n";
        return -1;
    }

    dashLine();
    otOut << "Encrypted:\n\n";
    cout << output << "\n";

    return 1;
}
