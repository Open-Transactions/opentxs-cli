// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdPairNode.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{

CmdPairNode::CmdPairNode()
{
    command = "pairnode";
    args[0] = "--mynym <nym>";
    args[1] = "--hisnym <bridge nym>";
    args[2] = "--password <server password>";
    category = catMisc;
    help = "Pair with a Stash Node";
}

std::int32_t CmdPairNode::runWithOptions()
{
    return run(getOption("mynym"), getOption("hisnym"), getOption("password"));
}

std::int32_t CmdPairNode::run(
    std::string mynym,
    std::string hisnym,
    std::string password)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    auto result = SwigWrap::Pair_Node(mynym, hisnym, password);

    if (result) { return 1; }

    return 0;
}
}  // namespace opentxs
