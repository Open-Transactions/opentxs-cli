// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdRenameNym.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

namespace opentxs
{

CmdRenameNym::CmdRenameNym()
{
    command = "renamenym";
    args[0] = "--mynym <nym>";
    args[1] = "--label <label>";
    category = catWallet;
    help = "Rename one of your own nyms and set appropriate claims.";
}

std::int32_t CmdRenameNym::runWithOptions()
{
    return run(getOption("mynym"), getOption("label"));
}

std::int32_t CmdRenameNym::run(std::string mynym, std::string label)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    if (!checkMandatory("label", label)) { return -1; }

    if (!SwigWrap::Rename_Nym(mynym, label, 1)) {
        otOut << "Error: cannot rename nym.\n";
        return -1;
    }

    return 1;
}
}  // namespace opentxs
