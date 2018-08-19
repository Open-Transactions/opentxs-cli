// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdContactName.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{
CmdContactName::CmdContactName()
{
    command = "contactname";
    args[0] = "--contact <nym>";
    category = catOtherUsers;
    help = "Retrieve the label for a contact";
}

std::int32_t CmdContactName::runWithOptions()
{
    return run(getOption("contact"));
}

std::int32_t CmdContactName::run(const std::string& id)
{
    if (id.empty()) { return -1; }

    const auto pContact =
        Opentxs::Client().Contacts().Contact(opentxs::Identifier::Factory(id));

    if (pContact) {
        const auto label = pContact->Label();

        otOut << label << std::endl;
        return 0;
    }

    return -1;
}
}  // namespace opentxs
