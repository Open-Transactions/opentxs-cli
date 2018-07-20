// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdPasswordEncrypt.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdPasswordEncrypt::CmdPasswordEncrypt()
{
    command = "passwordencrypt";
    category = catAdmin;
    help = "Password-encrypt a plaintext using a symmetric key.";
}

CmdPasswordEncrypt::~CmdPasswordEncrypt() {}

int32_t CmdPasswordEncrypt::runWithOptions() { return run(); }

int32_t CmdPasswordEncrypt::run()
{
    string key = inputText("a symmetric key");
    if ("" == key) { return -1; }

    string input = inputText("the plaintext");
    if ("" == input) { return -1; }

    string encrypted = SwigWrap::SymmetricEncrypt(key, input);
    if ("" == encrypted) {
        otOut << "Error: cannot encrypt plaintext.\n";
        return -1;
    }

    cout << encrypted << "\n";

    return 1;
}
