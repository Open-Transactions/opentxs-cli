// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdDecrypt.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdDecrypt::CmdDecrypt()
{
    command = "decrypt";
    args[0] = "--mynym <nym>";
    category = catAdmin;
    help = "Decrypt encrypted input text using mynym's private key.";
}

CmdDecrypt::~CmdDecrypt() {}

int32_t CmdDecrypt::runWithOptions() { return run(getOption("mynym")); }

int32_t CmdDecrypt::run(string mynym)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    string input = inputText("the encrypted text to be decrypted");
    if ("" == input) { return -1; }

    string output = SwigWrap::Decrypt(mynym, input);
    if ("" == output) {
        otOut << "Error: cannot decrypt input.\n";
        return -1;
    }

    dashLine();
    otOut << "Decrypted:\n\n";
    cout << output << "\n";

    return 1;
}
