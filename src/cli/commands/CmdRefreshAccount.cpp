// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdRefreshAccount.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdRefreshAccount::CmdRefreshAccount()
{
    command = "refreshaccount";
    args[0] = "--myacct <account>";
    category = catAccounts;
    help = "Download myacct's latest intermediary files.";
}

CmdRefreshAccount::~CmdRefreshAccount() {}

int32_t CmdRefreshAccount::runWithOptions() { return run(getOption("myacct")); }

int32_t CmdRefreshAccount::run(string myacct)
{
    if (!checkAccount("myacct", myacct)) { return -1; }

    string server = SwigWrap::GetAccountWallet_NotaryID(myacct);
    if ("" == server) {
        otOut << "Error: cannot determine server from myacct.\n";
        return -1;
    }

    string mynym = SwigWrap::GetAccountWallet_NymID(myacct);
    if ("" == mynym) {
        otOut << "Error: cannot determine mynym from myacct.\n";
        return -1;
    }

    {
        if (!OT::App().API().ServerAction().DownloadAccount(
                Identifier::Factory(mynym),
                Identifier::Factory(server),
                Identifier::Factory(myacct),
                true)) {
            otOut << "Error retrieving intermediary files for myacct.\n";
            return -1;
        }
    }

    return 1;
}
