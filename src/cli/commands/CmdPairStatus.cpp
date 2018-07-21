// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdPairStatus.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{

CmdPairStatus::CmdPairStatus()
{
    command = "pairstatus";
    args[0] = "--mynym <nym ID>";
    args[1] = "--issuer <issuer nym ID>";
    category = catMisc;
    help = "Pairing status";
}

std::int32_t CmdPairStatus::runWithOptions()
{
    return run(getOption("mynym"), getOption("issuer"));
}

std::int32_t CmdPairStatus::run(
    std::string localNym,
    const std::string& issuerNym)
{
    if (!checkNym("mynym", localNym)) { return -1; }

    std::cout << SwigWrap::Pair_Status(localNym, issuerNym);

    return 1;
}
}  // namespace opentxs
