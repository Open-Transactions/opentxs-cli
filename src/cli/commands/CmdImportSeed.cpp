// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdImportSeed.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

namespace opentxs
{
CmdImportSeed::CmdImportSeed()
{
    command = "importseed";
    args[0] = "--words <word list>";
    args[1] = "[--phrase <passphrase>]";
    category = catWallet;
    help = "Add a BIP39 seed to the wallet";
}

int32_t CmdImportSeed::runWithOptions()
{
    return run(getOption("words"), getOption("phrase"));
}

int32_t CmdImportSeed::run(const std::string& words, const std::string& phrase)
{
    if (words.empty()) { return -1; }

    const std::string fingerprint = SwigWrap::Wallet_ImportSeed(words, phrase);
    const bool empty = fingerprint.empty();

    if (!empty) { std::cout << fingerprint << std::endl; }

    return empty ? -1 : 0;
}
}  // namespace opentxs
