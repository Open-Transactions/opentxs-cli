// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdShowContact.hpp"

#include <opentxs/opentxs.hpp>

#define OT_METHOD "opentxs::CmdShowContact::"

namespace opentxs
{
CmdShowContact::CmdShowContact()
{
    command = "showcontact";
    args[0] = "--contact <id>";
    category = catOtherUsers;
    help = "Display contact data";
}

void CmdShowContact::display_groups(const ui::ContactSection& section) const
{
    auto group = section.First();

    if (false == group->Valid()) { return; }

    auto lastGroup = group->Last();
    LogNormal(OT_METHOD)(__FUNCTION__)(": ** ")(group->Name("en"))(" (")(
        group->Type())(")")
        .Flush();
    display_items(group);

    while (false == lastGroup) {
        group = section.Next();
        lastGroup = group->Last();
        LogNormal(OT_METHOD)(__FUNCTION__)(": ** ")(group->Name("en"))(" (")(
            group->Type())(")")
            .Flush();
        display_items(group);
    }
}

void CmdShowContact::display_items(const ui::ContactSubsection& group) const
{
    auto item = group.First();

    if (false == item->Valid()) { return; }

    auto lastItem = item->Last();
    LogNormal(OT_METHOD)(__FUNCTION__)(": * ID: ")(item->ClaimID())(" Value: ")(
        item->Value())(" Primary: ")(item->IsPrimary())(" Active: ")(
        item->IsActive())
        .Flush();

    while (false == lastItem) {
        item = group.Next();
        lastItem = item->Last();
        LogNormal(OT_METHOD)(__FUNCTION__)(": * ID: ")(item->ClaimID())(
            " Value: ")(item->Value())(" Primary: ")(item->IsPrimary())(
            " Active: ")(item->IsActive())
            .Flush();
    }
}

std::int32_t CmdShowContact::runWithOptions()
{
    return run(getOption("contact"));
}

std::int32_t CmdShowContact::run(const std::string& id)
{
    if (!checkMandatory("contact", id)) { return -1; }

    const OTIdentifier contactID = Identifier::Factory(id);
    auto& contact = Opentxs::Client().UI().Contact(contactID);
    LogNormal(OT_METHOD)(__FUNCTION__)(contact.DisplayName())(": (")(
        contact.ContactID())(") Payment Code: ")(contact.PaymentCode())
        .Flush();
    dashLine();
    auto section = contact.First();

    if (false == section->Valid()) { return 1; }

    auto lastSection = section->Last();
    LogNormal(OT_METHOD)(__FUNCTION__)(": * ")(section->Name("en"))(" (")(
        section->Type())(")")
        .Flush();
    display_groups(section);

    while (false == lastSection) {
        section = contact.Next();
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
