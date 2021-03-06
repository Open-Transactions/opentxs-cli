// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdMarkUnRead.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{
CmdMarkUnRead::CmdMarkUnRead()
{
    command = "markunread";
    args[0] = "--mynym <nym>";
    args[1] = "--thread <threadID>";
    args[2] = "--item <itemID>";
    category = catOtherUsers;
    help = "Mark a thread item as unread.";
}

std::int32_t CmdMarkUnRead::runWithOptions()
{
    return run(getOption("mynym"), getOption("thread"), getOption("item"));
}

std::int32_t CmdMarkUnRead::run(
    std::string mynym,
    const std::string& threadID,
    const std::string& itemID)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    const auto output = Opentxs::Client().Activity().MarkUnread(
        Identifier::Factory(mynym),
        Identifier::Factory(threadID),
        Identifier::Factory(itemID));

    if (output) { return 0; }

    return -1;
}
}  // namespace opentxs
