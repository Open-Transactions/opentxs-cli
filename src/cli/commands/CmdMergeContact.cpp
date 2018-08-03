// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdMergeContact.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{
CmdMergeContact::CmdMergeContact()
{
    command = "mergecontact";
    args[0] = "--id <id>";
    args[1] = "--into <id>";
    category = catOtherUsers;
    help = "Merge two contacts";
}

std::int32_t CmdMergeContact::runWithOptions()
{
    return run(getOption("into"), getOption("id"));
}

std::int32_t CmdMergeContact::run(
    const std::string& parent,
    const std::string& child)
{
    auto contact = OT::App().Client().Contacts().Merge(
        Identifier::Factory(parent), Identifier::Factory(child));

    if (false == bool(contact)) { return -1; }

    otOut << contact->Print();

    return 1;
}
}  // namespace opentxs
