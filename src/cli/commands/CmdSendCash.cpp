// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdSendCash.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdSendCash::CmdSendCash()
{
    command = "sendcash";
    args[0] = "[--server <server>]";
    args[1] = "[--mynym <nym>]";
    args[2] = "[--myacct <account>]";
    args[3] = "[--mypurse <purse>]";
    args[4] = "--hisnym <nym>";
    args[5] = "--amount <amount>";
    args[6] = "[--indices <indices|all>]";
    args[7] = "[--password <true|false>]";
    category = catOtherUsers;
    help = "Send cash from mypurse to recipient, withdraw if necessary.";
    usage = "Specify either myacct OR mypurse.\n"
            "When mypurse is specified server and mynym are mandatory.";
}

CmdSendCash::~CmdSendCash() {}

int32_t CmdSendCash::runWithOptions()
{
    return run(
        getOption("server"),
        getOption("mynym"),
        getOption("myacct"),
        getOption("mypurse"),
        getOption("hisnym"),
        getOption("amount"),
        getOption("indices"),
        getOption("password"));
}

int32_t CmdSendCash::run(
    string server,
    string mynym,
    string myacct,
    string mypurse,
    string hisnym,
    string amount,
    string indices,
    string password)
{
    if ("" != myacct) {
        if (!checkAccount("myacct", myacct)) { return -1; }

        // myacct specified: server and mynym are implied
        server = SwigWrap::GetAccountWallet_NotaryID(myacct);
        if ("" == server) {
            otOut << "Error: cannot determine server from myacct.\n";
            return -1;
        }

        mynym = SwigWrap::GetAccountWallet_NymID(myacct);
        if ("" == mynym) {
            otOut << "Error: cannot determine mynym from myacct.\n";
            return -1;
        }

        string assetType = getAccountAssetType(myacct);
        if ("" == assetType) { return -1; }

        if ("" != mypurse && mypurse != assetType) {
            otOut << "Error: myacct instrument definition does not match "
                     "mypurse.\n";
            return -1;
        }

        mypurse = assetType;
    } else {
        // we want either ONE OF myacct OR mypurse to be specified
        if (!checkMandatory("myacct or mypurse", mypurse)) { return -1; }

        if (!checkPurse("mypurse", mypurse)) { return -1; }

        // mypurse specified: server and mynym are mandatory
        if (!checkServer("server", server)) { return -1; }

        if (!checkNym("mynym", mynym)) { return -1; }
    }

    if (!checkNym("hisnym", hisnym)) { return -1; }

    if ("" != password && !checkFlag("password", password)) { return -1; }

    int64_t value = checkAmount("amount", amount, myacct);
    if (OT_ERROR_AMOUNT == value) { return -1; }

    // Below this point we can just try to pay it from the purse, and if unable
    // to, try to get the remaining funds from the account, IF that's available.

    string response = "";
    if (1 != sendCash(
                 response,
                 server,
                 mynym,
                 mypurse,
                 myacct,
                 hisnym,
                 amount,
                 indices,
                 password == "true")) {
        return -1;
    }

    cout << response << "\n";

    return 1;
}

int32_t CmdSendCash::sendCash(
    string& response,
    const string& server,
    const string& mynym,
    const string& assetType,
    const string& myacct,
    string& hisnym,
    const string& amount,
    string& indices,
    bool hasPassword) const
{
#if OT_CASH
    return Opentxs::Client().Cash().send_cash(
        response,
        server,
        mynym,
        assetType,
        myacct,
        hisnym,
        amount,
        indices,
        hasPassword);
#else
    return -1;
#endif  // OT_CASH
}
