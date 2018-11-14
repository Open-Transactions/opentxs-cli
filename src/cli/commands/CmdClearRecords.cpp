// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdClearRecords.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

#define OT_METHOD "opentxs::CmdClearRecords::"

using namespace opentxs;
using namespace std;

CmdClearRecords::CmdClearRecords()
{
    command = "clearrecords";
    args[1] = "--myacct <account>";
    category = catMisc;
    help = "Clear all archived records and receipts.";
}

CmdClearRecords::~CmdClearRecords() {}

int32_t CmdClearRecords::runWithOptions() { return run(getOption("myacct")); }

int32_t CmdClearRecords::run(string myacct)
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

    // Recordbox for mynym contains the old payments (in and out.)
    LogNormal(OT_METHOD)(__FUNCTION__)(
        ": Clearing archived Nym-related records...")
        .Flush();
    bool success = SwigWrap::ClearRecord(server, mynym, mynym, 0, true);

    dashLine();

    // Recordbox for myacct contains the old inbox receipts.
    LogNormal(OT_METHOD)(__FUNCTION__)(
        ": Clearing archived Account-related records...")
        .Flush();
    success |= SwigWrap::ClearRecord(server, mynym, myacct, 0, true);

    return success ? 1 : -1;
}
