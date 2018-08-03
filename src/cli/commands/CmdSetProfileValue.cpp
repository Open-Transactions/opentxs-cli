// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdSetProfileValue.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{
CmdSetProfileValue::CmdSetProfileValue()
{
    command = "setprofilevalue";
    args[0] = "--mynym <id>";
    args[1] = "--section <section>";
    args[2] = "--type <type>";
    args[3] = "--value <data>";
    category = catNyms;
    help = "Add data to a profile";
}

std::int32_t CmdSetProfileValue::runWithOptions()
{
    return run(
        getOption("mynym"),
        getOption("section"),
        getOption("type"),
        getOption("value"));
}

std::int32_t CmdSetProfileValue::run(
    std::string mynym,
    const std::string& section,
    const std::string& type,
    const std::string& value)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    const OTIdentifier nymID = Identifier::Factory({mynym});
    auto& profile = OT::App().Client().UI().Profile(nymID);

    const auto sectionType =
        static_cast<proto::ContactSectionName>(std::stoi(section));
    const auto itemType = static_cast<proto::ContactItemType>(std::stoi(type));

    if (false == section_exists(profile.AllowedSections("en"), sectionType)) {
        otErr << "Invalid section." << std::endl;

        return -1;
    }

    if (false ==
        section_exists(profile.AllowedItems(sectionType, "en"), itemType)) {
        otErr << "Invalid type." << std::endl;

        return -1;
    }

    const auto isSet =
        profile.AddClaim(sectionType, itemType, value, true, true);

    if (isSet) {
        otErr << "Success" << std::endl;

        return 1;
    }

    return 1;
}
}  // namespace opentxs
