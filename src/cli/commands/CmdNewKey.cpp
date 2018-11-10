// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdNewKey.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

#define OT_METHOD "opentxs::CmdNewNewKey::"

using namespace opentxs;
using namespace std;

CmdNewKey::CmdNewKey()
{
    command = "newkey";
    category = catAdmin;
    help = "Create a new symmetric key.";
}

CmdNewKey::~CmdNewKey() {}

int32_t CmdNewKey::runWithOptions() { return run(); }

int32_t CmdNewKey::run()
{
    string key = SwigWrap::CreateSymmetricKey();
    if ("" == key) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot create new key.")
            .Flush();
        return -1;
    }

    cout << key << "\n";

    return 1;
}
