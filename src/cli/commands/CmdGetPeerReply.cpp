// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdGetPeerReply.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{

CmdGetPeerReply::CmdGetPeerReply()
{
    command = "getpeerreply";
    args[0] = "--mynym <nym>";
    args[1] = "--reply <reply ID>";
    category = catOtherUsers;
    help = "Show a base64-encoded peer reply";
}

std::int32_t CmdGetPeerReply::runWithOptions()
{
    return run(getOption("mynym"), getOption("reply"));
}

std::int32_t CmdGetPeerReply::run(std::string mynym, std::string reply)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    if (reply.empty()) { return -1; }

    const auto text = SwigWrap::getReply_Base64(mynym, reply);

    otOut << "Peer reply ID: " << reply << std::endl;
    otOut << text << std::endl;

    return 1;
}
}  // namespace opentxs
