// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdAddContact.hpp"

#include <opentxs/opentxs.hpp>

#define OT_METHOD "opentxs::CmdAddContact::"

namespace opentxs
{
CmdAddContact::CmdAddContact()
{
    command = "addcontact";
    args[0] = "--hisnym <nym>";
    args[1] = "--label [<label>]";
    category = catOtherUsers;
    help = "Add a new contact based on Nym ID (or payment code)";
    usage = "Specify a nym id or payment code for hisnym.";
}

std::int32_t CmdAddContact::runWithOptions()
{
    return run(getOption("hisnym"), getOption("label"));
}

std::int32_t CmdAddContact::run(
    const std::string& hisnym,
    const std::string& label)
{
    if (hisnym.empty()) { return -1; }

    OTIdentifier nymID = Identifier::Factory(String::Factory(hisnym.c_str()));
    auto code = Opentxs::Client().Factory().PaymentCode(hisnym);

    if (nymID->empty()) {
        LogOutput(OT_METHOD)(__FUNCTION__)(
            ": Provided ID was not a nymID.").Flush();
    } else {
        LogOutput(OT_METHOD)(__FUNCTION__)(
            ": Provided ID was a nymID.").Flush();
    }

    if (false == code->VerifyInternally()) {
        LogOutput(OT_METHOD)(__FUNCTION__)(
            ": Provided ID was not a payment code.").Flush();
    } else {
        LogOutput(OT_METHOD)(__FUNCTION__)(
            ": Provided ID was a payment code.").Flush();
    }

    if (nymID->empty() && code->VerifyInternally()) {
        nymID = code->ID();
        LogOutput(OT_METHOD)(__FUNCTION__)(
            ": Derived nymID: ")(nymID)(".").Flush();
    }

    const auto response =
        Opentxs::Client().Contacts().NewContact(label, nymID, code);

    if (response) { return 0; }

    return -1;
}
}  // namespace opentxs
