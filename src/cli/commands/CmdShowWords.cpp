// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdShowWords.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

namespace opentxs
{
CmdShowWords::CmdShowWords()
{
    command = "showwords";
    category = catWallet;
    help = "Show wallet BIP39 seed as a word list.";
}

CmdShowWords::~CmdShowWords() {}

int32_t CmdShowWords::runWithOptions() { return run(); }

int32_t CmdShowWords::run()
{
    const std::string words = SwigWrap::Wallet_GetWords();
    const bool empty = 1 > words.size();
    std::cout << words << std::endl;

    return empty ? -1 : 0;
}
}  // namespace opentxs
