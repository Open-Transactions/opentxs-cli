// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdGetPeerReplies.hpp"

#include <opentxs/opentxs.hpp>

#define OT_METHOD "opentxs::CmdGetPeerReplies::"

namespace opentxs
{

CmdGetPeerReplies::CmdGetPeerReplies()
{
    command = "getpeerreplies";
    args[0] = "--mynym <nym>";
    category = catOtherUsers;
    help = "Get a list of incoming peer reply IDs";
}

CmdGetPeerReplies::~CmdGetPeerReplies() {}

std::int32_t CmdGetPeerReplies::runWithOptions()
{
    return run(getOption("mynym"));
}

std::int32_t CmdGetPeerReplies::run(std::string mynym)
{
    if (!checkNym("mynym", mynym)) { return -1; }
    {

        auto sent = SwigWrap::getSentReplies(mynym);
        auto incoming = SwigWrap::getIncomingReplies(mynym);
        auto finished = SwigWrap::getFinishedReplies(mynym);
        auto processed = SwigWrap::getProcessedReplies(mynym);

        LogNormal(OT_METHOD)(__FUNCTION__)(" : Peer reply box contents:")
            .Flush();
        LogNormal(OT_METHOD)(__FUNCTION__)(" : Sent box:").Flush();
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
