// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdCanMessage.hpp"

#include <opentxs/opentxs.hpp>

#define OT_METHOD "opentxs::CmdCanMessage::"

namespace opentxs
{
CmdCanMessage::CmdCanMessage()
{
    command = "canmessage";
    args[0] = "--sender <nym>";
    args[1] = "--recipient <nym>";
    category = catOtherUsers;
    help = "Determine if prerequisites for messaging are met";
}

int32_t CmdCanMessage::runWithOptions()
{
    return run(getOption("sender"), getOption("recipient"));
}

int32_t CmdCanMessage::run(
    const std::string& sender,
    const std::string& recipient)
{
    if (sender.empty()) { return -1; }

    if (recipient.empty()) { return -1; }
    {

        const auto response = Opentxs::Client().OTX().CanMessage(
            Identifier::Factory(sender), Identifier::Factory(recipient));
        LogNormal(OT_METHOD)(__FUNCTION__)(
            std::to_string(static_cast<std::int8_t>(response)))
            .Flush();

        return 0;
    }
}
}  // namespace opentxs
