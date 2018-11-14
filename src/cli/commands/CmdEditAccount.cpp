// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdEditAccount.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

#define OT_METHOD "opentxs::CmdEditAccount::"

using namespace opentxs;
using namespace std;

CmdEditAccount::CmdEditAccount()
{
    command = "editaccount";
    args[0] = "--myacct <account>";
    args[1] = "--label <label>";
    category = catWallet;
    help = "Edit myacct's label, as it appears in your wallet.";
}

CmdEditAccount::~CmdEditAccount() {}

int32_t CmdEditAccount::runWithOptions()
{
    return run(getOption("myacct"), getOption("label"));
}

int32_t CmdEditAccount::run(string myacct, string label)
{
    if (!checkAccount("myacct", myacct)) { return -1; }

    if (!checkMandatory("label", label)) { return -1; }

    string mynym = SwigWrap::GetAccountWallet_NymID(myacct);
    if ("" == mynym) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot determine mynym from myacct.")
            .Flush();
        return -1;
    }

    if (!SwigWrap::SetAccountWallet_Name(myacct, mynym, label)) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot set account label.")
            .Flush();
        return -1;
    }

    return 1;
}
