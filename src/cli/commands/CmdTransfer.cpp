// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdTransfer.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdTransfer::CmdTransfer()
{
    command = "transfer";
    args[0] = "--myacct <account>";
    args[1] = "--hisacct <account>";
    args[2] = "--amount <amount>";
    args[3] = "[--memo <memoline>]";
    category = catAccounts;
    help = "Send a transfer from myacct to hisacct.";
}

CmdTransfer::~CmdTransfer() {}

int32_t CmdTransfer::runWithOptions()
{
    return run(
        getOption("myacct"),
        getOption("hisacct"),
        getOption("amount"),
        getOption("memo"));
}

int32_t CmdTransfer::run(
    string myacct,
    string hisacct,
    string amount,
    string memo)
{

    if (!checkAccount("myacct", myacct)) { return -1; }

    if (!checkAccount("hisacct", hisacct)) { return -1; }

    int64_t value = checkAmount("amount", amount, myacct);
    if (OT_ERROR_AMOUNT == value) { return -1; }

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

    string hisServer = SwigWrap::GetAccountWallet_NotaryID(hisacct);
    if (hisServer != server) {
        otOut << "Error: myacct and hisacct are on different servers.\n";
        return -1;
    }

    if ("" == hisServer) {
        otOut << "Assuming hisaccount is on the same server as myacct.\n";
    }

    auto& sync = Opentxs::Client().Sync();

    OTIdentifier taskID = sync.SendTransfer(
        Identifier::Factory(mynym),
        Identifier::Factory(server),
        Identifier::Factory(myacct),
        Identifier::Factory(hisacct),
        value,
        memo);

    ThreadStatus status = sync.Status(taskID);
    while (status == ThreadStatus::RUNNING) {
        Log::Sleep(std::chrono::milliseconds(100));
        status = sync.Status(taskID);
    }

    switch (status) {
        case ThreadStatus::FINISHED_SUCCESS: {
            otOut << "Transfer completed successfully " << std::endl;
        } break;
        case ThreadStatus::FINISHED_FAILED: {
            otOut << "Transfer not completed " << std::endl;
            [[fallthrough]];
        }
        case ThreadStatus::ERROR:
        case ThreadStatus::SHUTDOWN:
        default: {
            return -1;
        }
    }

    {
        if (!Opentxs::Client().ServerAction().DownloadAccount(
                Identifier::Factory(mynym),
                Identifier::Factory(server),
                Identifier::Factory(myacct),
                true)) {
            otOut << "Error retrieving intermediary files for account.\n";
            return -1;
        }
    }

    return 1;
}
