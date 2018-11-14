// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdGetPeerRequest.hpp"

#include <opentxs/opentxs.hpp>

#define OT_METHOD "opentxs::CmdGetPeerRequest::"

namespace opentxs
{

CmdGetPeerRequest::CmdGetPeerRequest()
{
    command = "getpeerrequest";
    args[0] = "--mynym <nym>";
    args[1] = "--request <request ID>";
    category = catOtherUsers;
    help = "Show a base64-encoded peer request";
}

std::int32_t CmdGetPeerRequest::runWithOptions()
{
    return run(getOption("mynym"), getOption("request"));
}

std::int32_t CmdGetPeerRequest::run(std::string mynym, std::string request)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    if (request.empty()) { return -1; }
    {

        const auto text = SwigWrap::getRequest_Base64(mynym, request);

        LogNormal(OT_METHOD)(__FUNCTION__)(": Peer request ID: ")(request)
            .Flush();
        LogNormal(OT_METHOD)(__FUNCTION__)(text).Flush();

        return 1;
    }
}
}  // namespace opentxs
