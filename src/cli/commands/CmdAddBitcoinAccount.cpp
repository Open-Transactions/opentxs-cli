// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdAddBitcoinAccount.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{
CmdAddBitcoinAccount::CmdAddBitcoinAccount()
{
    command = "addbitcoinaccount";
    args[0] = "--mynym <nym>";
    category = catBlockchain;
    help = "Instantiate a BIP-44 account";
}

std::int32_t CmdAddBitcoinAccount::runWithOptions()
{
    return run(getOption("mynym"));
}

std::int32_t CmdAddBitcoinAccount::run(std::string mynym)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    const auto account = Opentxs::Client().Blockchain().NewAccount(
        Identifier::Factory(mynym),
        BlockchainAccountType::BIP44,
        proto::CITEMTYPE_BTC);
    otOut << "BIP-44 account ID: " << String::Factory(account) << std::endl;

    return 0;
}
}  // namespace opentxs
