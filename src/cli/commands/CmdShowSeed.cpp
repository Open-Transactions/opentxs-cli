// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdShowSeed.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

namespace opentxs
{
CmdShowSeed::CmdShowSeed()
{
    command = "showseed";
    category = catWallet;
    help = "Show the wallet BIP32 seed as a hex string.";
}

CmdShowSeed::~CmdShowSeed() {}

int32_t CmdShowSeed::runWithOptions() { return run(); }

int32_t CmdShowSeed::run()
{
    const std::string seed = SwigWrap::Wallet_GetSeed();
    const bool empty = 1 > seed.size();
    std::cout << seed << std::endl;

    return empty ? -1 : 0;
}
}  // namespace opentxs
