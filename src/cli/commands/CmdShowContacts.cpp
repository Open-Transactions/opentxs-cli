// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdShowContacts.hpp"

#include <opentxs/opentxs.hpp>

#define OT_METHOD "opentxs::CmdShowContacts::"

namespace opentxs
{

CmdShowContacts::CmdShowContacts()
{
    command = "showcontacts";
    args[0] = "--mynym <nym>";
    category = catOtherUsers;
    help = "Show the contact list for a nym in the wallet.";
}

std::int32_t CmdShowContacts::runWithOptions()
{
    return run(getOption("mynym"));
}

std::int32_t CmdShowContacts::run(std::string mynym)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    const OTIdentifier nymID = Identifier::Factory({mynym});
    auto& list = Opentxs::Client().UI().ContactList(nymID);
    LogNormal(OT_METHOD)(__FUNCTION__)(": Contacts:").Flush();
    dashLine();
    auto line = list.First();
    auto last = line->Last();
    LogNormal(OT_METHOD)(__FUNCTION__)(": ")(line->Section())(" ")(
        line->DisplayName())(" (")(line->ContactID())(")")
        .Flush();

    while (false == last) {
        line = list.Next();
        last = line->Last();
        LogNormal(OT_METHOD)(__FUNCTION__)(": ")(line->Section())("  ")(
            line->DisplayName())(" (")(line->ContactID())(")")
            .Flush();
    }

    LogNormal(OT_METHOD)(__FUNCTION__)(" ").Flush();

    return 1;
}
}  // namespace opentxs
