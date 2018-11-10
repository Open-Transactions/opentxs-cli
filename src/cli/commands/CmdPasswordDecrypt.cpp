// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdPasswordDecrypt.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

#define OT_METHOD "opentxs::CmdPasswordDecrypt::"

using namespace opentxs;
using namespace std;

CmdPasswordDecrypt::CmdPasswordDecrypt()
{
    command = "passworddecrypt";
    category = catAdmin;
    help = "Password-decrypt a ciphertext using a symmetric key.";
}

CmdPasswordDecrypt::~CmdPasswordDecrypt() {}

int32_t CmdPasswordDecrypt::runWithOptions() { return run(); }

int32_t CmdPasswordDecrypt::run()
{
    string key = inputText("a symmetric key");
    if ("" == key) { return -1; }

    string input = inputText("the encrypted text");
    if ("" == input) { return -1; }

    string encrypted = SwigWrap::SymmetricDecrypt(key, input);
    if ("" == encrypted) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot decrypt encrypted text.")
            .Flush();
        return -1;
    }

    cout << encrypted << "\n";

    return 1;
}
