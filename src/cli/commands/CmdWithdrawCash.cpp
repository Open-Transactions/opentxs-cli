// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdWithdrawCash.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdWithdrawCash::CmdWithdrawCash()
{
    command = "withdraw";
    args[0] = "--myacct <account>";
    args[1] = "--amount <amount>";
    category = catInstruments;
    help = "Withdraw from myacct as cash into local purse.";
}

CmdWithdrawCash::~CmdWithdrawCash() {}

int32_t CmdWithdrawCash::runWithOptions()
{
    return run(getOption("myacct"), getOption("amount"));
}

int32_t CmdWithdrawCash::run(string myacct, string amount)
{
    if (!checkAccount("myacct", myacct)) { return -1; }

    int64_t value = checkAmount("amount", amount, myacct);
    if (OT_ERROR_AMOUNT == value) { return -1; }

    return withdrawCash(myacct, value);
}

int32_t CmdWithdrawCash::withdrawCash(const string& myacct, int64_t amount)
    const
{
#if OT_CASH
    return Opentxs::Client().Cash().easy_withdraw_cash(myacct, amount);
#else
    return -1;
#endif  // OT_CASH
}
