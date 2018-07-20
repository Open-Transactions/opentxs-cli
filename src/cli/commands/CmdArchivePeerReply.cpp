// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdArchivePeerReply.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{

CmdArchivePeerReply::CmdArchivePeerReply()
{
    command = "archivepeerreply";
    args[0] = "--mynym <nym>";
    args[1] = "--mypurse <reply ID>";
    category = catOtherUsers;
    help = "Move a reply to the processed box";
}

std::int32_t CmdArchivePeerReply::runWithOptions()
{
    return run(getOption("mynym"), getOption("mypurse"));
}

std::int32_t CmdArchivePeerReply::run(std::string mynym, std::string mypurse)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    return OT::App().API().Exec().completePeerRequest(mynym, mypurse);
}
}  // namespace opentxs
