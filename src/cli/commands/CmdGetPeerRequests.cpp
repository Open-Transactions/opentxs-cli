// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdGetPeerRequests.hpp"

#include <opentxs/opentxs.hpp>

#define OT_METHOD "opentxs::CmdGetPeerRequests::"

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
    {

        auto sent = SwigWrap::getSentRequests(mynym);
        auto incoming = SwigWrap::getIncomingRequests(mynym);
        auto finished = SwigWrap::getFinishedRequests(mynym);
        auto processed = SwigWrap::getProcessedRequests(mynym);

        LogNormal(OT_METHOD)(__FUNCTION__)(": Peer request box contents:")
            .Flush();
        LogNormal(OT_METHOD)(__FUNCTION__)(": Sent box:").Flush();
        LogNormal(OT_METHOD)(__FUNCTION__)(sent).Flush();
        LogNormal(OT_METHOD)(__FUNCTION__)(": Incoming box:").Flush();
        LogNormal(OT_METHOD)(__FUNCTION__)(incoming).Flush();
        LogNormal(OT_METHOD)(__FUNCTION__)(": Finished box:").Flush();
        LogNormal(OT_METHOD)(__FUNCTION__)(finished).Flush();
        LogNormal(OT_METHOD)(__FUNCTION__)(": Processed box:").Flush();
        LogNormal(OT_METHOD)(__FUNCTION__)(processed).Flush();

        return 1;
    }
}
}  // namespace opentxs
