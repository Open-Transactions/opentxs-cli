// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdPreloadActivity.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{
CmdPreloadActivity::CmdPreloadActivity()
{
    command = "preloadactivity";
    args[0] = "--mynym <nym>";
    args[1] = "--items <count>";
    category = catOtherUsers;
    help = "Cache messages in all of a nym's threads.";
}

std::int32_t CmdPreloadActivity::runWithOptions()
{
    return run(getOption("mynym"), getOption("items"));
}

std::int32_t CmdPreloadActivity::run(
    std::string mynym,
    const std::string& items)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    std::size_t count{0};

    try {
        count = std::stoul(items);
    } catch (const std::out_of_range&) {
        count = 1;
    } catch (const std::invalid_argument&) {
        count = 1;
    }

    OT::App().Client().Activity().PreloadActivity(
        Identifier::Factory(mynym), count);

    return 0;
}
}  // namespace opentxs
