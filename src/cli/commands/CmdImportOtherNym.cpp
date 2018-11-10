// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdImportOtherNym.hpp"

#include <opentxs/opentxs.hpp>

#include <iostream>
#include <string>

#define OT_METHOD "opentxs::CmdImportOtherNym::"

namespace opentxs
{
CmdImportOtherNym::CmdImportOtherNym()
{
    command = "importothernym";
    category = catWallet;
    help = "Import a pasted nym.";
}

std::int32_t CmdImportOtherNym::runWithOptions() { return run(); }

std::int32_t CmdImportOtherNym::run()
{
    std::string input = inputText("an exported nym");

    if ("" == input) { return -1; }

    std::string outNym = SwigWrap::Import_Nym(input);

    if ("" == outNym) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot import Nym.")
            .Flush();

        return -1;
    }

    std::cout << "Imported nym: " << outNym << ".\n";

    return 1;
}
}  // namespace opentxs
