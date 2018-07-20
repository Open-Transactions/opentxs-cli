// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdNewAccount.hpp"
#include "CmdRegisterNym.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <string>

using namespace opentxs;
using namespace std;

CmdNewAccount::CmdNewAccount()
{
    command = "newaccount";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--mypurse <purse>";
    category = catAccounts;
    help = "Create a new asset account.";
}

CmdNewAccount::~CmdNewAccount() {}

int32_t CmdNewAccount::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("mypurse"));
}

int32_t CmdNewAccount::run(string server, string mynym, string mypurse)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    if (!checkPurse("mypurse", mypurse)) { return -1; }

    if (!SwigWrap::IsNym_RegisteredAtServer(mynym, server)) {
        CmdRegisterNym registerNym;
        registerNym.run(server, mynym, "true", "false");
    }

    std::string response;
    {
        response =
            OT::App()
                       .API()
                       .ServerAction()
                       .RegisterAccount(
                           Identifier::Factory(mynym),
                           Identifier::Factory(server),
                           Identifier::Factory(mypurse))
                       ->Run();
    }
    return processResponse(response, "create asset account");
}
