// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdDepositCheques.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{
CmdDepositCheques::CmdDepositCheques()
{
    command = "depositcheques";
    args[0] = "--mynym <nym>";
    category = catWallet;
    help = "Deposit all undeposited cheques";
}

std::int32_t CmdDepositCheques::runWithOptions()
{
    return run(getOption("mynym"));
}

std::int32_t CmdDepositCheques::run(std::string mynym)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    return OT::App().API().Sync().DepositCheques(Identifier::Factory(mynym));
}
}  // namespace opentxs
