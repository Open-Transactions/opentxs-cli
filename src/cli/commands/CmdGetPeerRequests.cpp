// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdGetPeerRequests.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{

CmdGetPeerRequests::CmdGetPeerRequests()
{
    command = "getpeerrequests";
    args[0] = "--mynym <nym>";
    category = catOtherUsers;
    help = "Get a list of incoming peer request IDs";
}

CmdGetPeerRequests::~CmdGetPeerRequests() {}

std::int32_t CmdGetPeerRequests::runWithOptions()
{
    return run(getOption("mynym"));
}

std::int32_t CmdGetPeerRequests::run(std::string mynym)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    auto sent = SwigWrap::getSentRequests(mynym);
    auto incoming = SwigWrap::getIncomingRequests(mynym);
    auto finished = SwigWrap::getFinishedRequests(mynym);
    auto processed = SwigWrap::getProcessedRequests(mynym);

    otOut << "Peer request box contents:" << std::endl;
    otOut << "Sent box:" << std::endl;
    otOut << sent << std::endl;
    otOut << "Incoming box:" << std::endl;
    otOut << incoming << std::endl;
    otOut << "Finished box:" << std::endl;
    otOut << finished << std::endl;
    otOut << "Processed box:" << std::endl;
    otOut << processed << std::endl;

    return 1;
}
}  // namespace opentxs
