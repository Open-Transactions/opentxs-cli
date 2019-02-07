// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdRefresh.hpp"
#include "CmdRefreshNym.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

#define OT_METHOD "opentxs::CmdRefresh::"

using namespace opentxs;
using namespace std;

CmdRefresh::CmdRefresh()
{
    command = "refresh";
    args[0] = "--myacct <account>";
    category = catWallet;
    help = "Performs both refreshnym and refreshaccount.";
}

CmdRefresh::~CmdRefresh() {}

int32_t CmdRefresh::runWithOptions() { return run(getOption("myacct")); }

int32_t CmdRefresh::run(string myacct)
{
    if (!checkAccount("myacct", myacct)) { return -1; }

    string server = SwigWrap::GetAccountWallet_NotaryID(myacct);
    if ("" == server) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot determine server from myacct.")
            .Flush();
        return -1;
    }

    string mynym = SwigWrap::GetAccountWallet_NymID(myacct);
    if ("" == mynym) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot determine mynym from myacct.")
            .Flush();
        return -1;
    }

    CmdRefreshNym refreshNym;
    if (0 > refreshNym.run(server, mynym)) { return -1; }

    auto task = Opentxs::Client().OTX().ProcessInbox(
        Identifier::Factory(mynym),
        Identifier::Factory(server),
        Identifier::Factory(myacct));

    const auto result = std::get<1>(task).get();
    
    const auto success = CmdBase::GetResultSuccess(result);
    if (false == success) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error retrieving intermediary "
                                           "files for myacct.")
            .Flush();
        return -1;
    }

    return 1;
}
