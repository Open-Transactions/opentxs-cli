// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdAddContact.hpp"

#include <opentxs/opentxs.hpp>

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

    OTIdentifier nymID = Identifier::Factory(String(hisnym.c_str()));
    auto code = OT::App().Client().Factory().PaymentCode(hisnym);

    if (nymID->empty()) {
        otErr << "Provided ID was not a nymID." << std::endl;
    } else {
        otErr << "Provided ID was a nymID." << std::endl;
    }

    if (false == code->VerifyInternally()) {
        otErr << "Provided ID was not a payment code." << std::endl;
    } else {
        otErr << "Provided ID was a payment code." << std::endl;
    }

    if (nymID->empty() && code->VerifyInternally()) {
        nymID = code->ID();
        otErr << "Derived nymID: " << String(nymID) << std::endl;
    }

    const auto response =
        OT::App().Client().Contacts().NewContact(label, nymID, code);

    if (response) { return 0; }

    return -1;
}
}  // namespace opentxs
