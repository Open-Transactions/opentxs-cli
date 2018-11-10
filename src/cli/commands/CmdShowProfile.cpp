// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdShowProfile.hpp"

#include <opentxs/opentxs.hpp>

#define OT_METHOD "opentxs::CmdShowProfile::"

namespace opentxs
{
CmdShowProfile::CmdShowProfile()
{
    command = "showprofile";
    args[0] = "--mynym <id>";
    category = catNyms;
    help = "Display profile data";
}

void CmdShowProfile::display_groups(const ui::ProfileSection& section) const
{
    auto group = section.First();

    if (false == group->Valid()) { return; }

    auto lastGroup = group->Last();
    LogNormal(OT_METHOD)(__FUNCTION__)(": ** ")(group->Name("en"))(" (")(
        group->Type())(")")
        .Flush();
    (display_items(group));

    while (false == lastGroup) {
        group = section.Next();
        lastGroup = group->Last();
        LogNormal(OT_METHOD)(__FUNCTION__)(": ** ")(group->Name("en"))(" (")(
            group->Type())(")")
            .Flush();
        display_items(group);
    }
}

void CmdShowProfile::display_items(const ui::ProfileSubsection& group) const
{
    auto item = group.First();

    if (false == item->Valid()) { return; }

    auto lastItem = item->Last();
    LogNormal(OT_METHOD)(__FUNCTION__)(":   * ID: ")(item->ClaimID())(
        " Value: ")(item->Value())(" Primary: ")(item->IsPrimary())(
        " Active: ")(item->IsActive())
        .Flush();

    while (false == lastItem) {
        item = group.Next();
        lastItem = item->Last();
        LogNormal(OT_METHOD)(__FUNCTION__)(":  * ID: ")(item->ClaimID())(
            " Value: ")(item->Value())(" Primary: ")(item->IsPrimary())(
            " Active: ")(item->IsActive())
            .Flush();
    }
}

std::int32_t CmdShowProfile::runWithOptions()
{
    return run(getOption("mynym"));
}

std::int32_t CmdShowProfile::run(std::string mynym)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    const OTIdentifier nymID = Identifier::Factory({mynym});
    auto& profile = Opentxs::Client().UI().Profile(nymID);
    LogNormal(OT_METHOD)(__FUNCTION__)(profile.DisplayName())(": (")(
        profile.ID())(") Payment Code: ")(profile.PaymentCode())
        .Flush();
    dashLine();
    auto section = profile.First();

    if (false == section->Valid()) { return 1; }

    auto lastSection = section->Last();
    LogNormal(OT_METHOD)(__FUNCTION__)(": * ")(section->Name("en"))(" (")(
        section->Type())(")")
        .Flush();
    display_groups(section);

    while (false == lastSection) {
        section = profile.Next();
        lastSection = section->Last();
        LogNormal(OT_METHOD)(__FUNCTION__)(": * ")(section->Name("en"))(" (")(
            section->Type())(")")
            .Flush();
        display_groups(section);
    }

    LogNormal(OT_METHOD)(__FUNCTION__)(" ").Flush();

    return 1;
}
}  // namespace opentxs
