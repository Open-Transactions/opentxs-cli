// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdGetContact.hpp"

#include <opentxs/opentxs.hpp>

#define OT_METHOD "opentxs::CmdGetContact::"

namespace opentxs
{
CmdGetContact::CmdGetContact()
{
    command = "getcontact";
    args[0] = "--hisnym <nymid>";
    category = catOtherUsers;
    help = "Look up the contact ID for a nym ID";
}

std::int32_t CmdGetContact::runWithOptions()
{
    return run(getOption("hisnym"));
}

std::int32_t CmdGetContact::run(const std::string& hisnym)
{
    const auto contact = SwigWrap::Nym_to_Contact(hisnym);

    if (contact.empty()) {
        LogOutput(OT_METHOD)(__FUNCTION__)(
            ": Nym ")(hisnym)(" is not associated with a contact.").Flush();
    } else {
        LogOutput(OT_METHOD)(__FUNCTION__)(": ")(contact)(".").Flush();
    }

    return 0;
}
}  // namespace opentxs
