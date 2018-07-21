// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdAcceptIncoming.hpp"

#include <opentxs/opentxs.hpp>

#include <cstdint>
#include <string>

namespace opentxs
{
CmdAcceptIncoming::CmdAcceptIncoming()
{
    command = "acceptincoming";
    args[0] = "--myacct <account>";
    category = catAccounts;
    help = "Accept all incoming payments in myacct's inbox.";
}

std::int32_t CmdAcceptIncoming::runWithOptions()
{
    return run(getOption("myacct"));
}

std::int32_t CmdAcceptIncoming::run(std::string myacct)
{
    if (!checkAccount("myacct", myacct)) { return -1; }

    const auto& storage = OT::App().DB();
    const auto accountID = Identifier::Factory(myacct);
    const auto nymID = storage.AccountOwner(accountID);

    if (nymID->empty()) { return -1; }

    const auto serverID = storage.AccountServer(accountID);

    if (serverID->empty()) { return -1; }

    const auto output =
        OT::App().API().Sync().AcceptIncoming(nymID, accountID, serverID);

    if (!output) {
        otErr << "Failed to accept incoming payments." << std::endl;

        return -1;
    }

    return 0;
}
}  // namespace opentxs
