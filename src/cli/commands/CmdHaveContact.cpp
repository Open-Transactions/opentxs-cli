// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdHaveContact.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{
CmdHaveContact::CmdHaveContact()
{
    command = "havecontact";
    args[0] = "--contact <nym or payment code>";
    category = catOtherUsers;
    help = "Determine if a contact exists";
}

std::int32_t CmdHaveContact::runWithOptions()
{
    return run(getOption("contact"));
}

std::int32_t CmdHaveContact::run(const std::string& id)
{
    if (id.empty()) { return -1; }

    const auto contact = Opentxs::Client().Contacts().Contact(
        Identifier::Factory(String::Factory(id.c_str())));

    if (contact) {
        otOut << "true" << std::endl;
    } else {
        otOut << "false" << std::endl;
    }

    return 0;
}
}  // namespace opentxs
