// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdVerifyReceipt.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdVerifyReceipt::CmdVerifyReceipt()
{
    command = "verifyreceipt";
    args[0] = "--server <server>";
    args[1] = "--myacct <account>";
    category = catAccounts;
    help = "Verify your intermediary files against last signed receipt.";
}

CmdVerifyReceipt::~CmdVerifyReceipt() {}

int32_t CmdVerifyReceipt::runWithOptions()
{
    return run(getOption("server"), getOption("myacct"));
}

int32_t CmdVerifyReceipt::run(string server, string myacct)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkAccount("myacct", myacct)) { return -1; }

    string mynym = SwigWrap::GetAccountWallet_NymID(myacct);
    if ("" == mynym) {
        otOut << "Error: cannot determine mynym from myacct.\n";
        return -1;
    }

    if (!SwigWrap::VerifyAccountReceipt(server, mynym, myacct)) {
        otOut << "Error: cannot verify recepit.\n";
        return -1;
    }
    return 1;
}
