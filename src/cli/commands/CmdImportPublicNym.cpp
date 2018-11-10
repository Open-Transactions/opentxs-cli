// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdImportPublicNym.hpp"

#include <opentxs/opentxs.hpp>

#include <string>

#define OT_METHOD "opentxs::CmdImportPublicNym::"

namespace opentxs
{

CmdImportPublicNym::CmdImportPublicNym()
{
    command = "importpublicnym";
    category = catWallet;
    help = "Import a public nym.";
}

std::int32_t CmdImportPublicNym::runWithOptions() { return run(); }

std::int32_t CmdImportPublicNym::run()
{
    const auto input = inputText("An armored public nym");

    if (input.empty()) { return -1; }

    const auto nymID = SwigWrap::Import_Nym(input);

    if (nymID.empty()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot import Nym.")
            .Flush();

        return -1;
    }

    LogNormal(OT_METHOD)(__FUNCTION__)(": Imported nym: ")(nymID)(".").Flush();

    return 1;
}
}  // namespace opentxs
