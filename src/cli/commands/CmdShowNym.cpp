// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdShowNym.hpp"

#include <opentxs/opentxs.hpp>

#include <iostream>

namespace opentxs
{
CmdShowNym::CmdShowNym()
{
    command = "shownym";
    args[0] = "--mynym <nym>";
    category = catNyms;
    help = "Show mynym's statistics.";
}

std::int32_t CmdShowNym::runWithOptions() { return run(getOption("mynym")); }

std::int32_t CmdShowNym::run(std::string mynym)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    std::string nymStats = SwigWrap::GetNym_Stats(mynym);

    if ("" == nymStats) {
        nymStats = "This nym is not located in the local wallet.";
    }

    std::string claims = SwigWrap::DumpContactData(mynym);
    std::cout << nymStats << std::endl << claims;
    auto nym = Opentxs::Client().Wallet().Nym(Identifier::Factory(mynym));

    if (nym) {
        const auto armored =
            proto::ProtoAsArmored(nym->asPublicNym(), "PUBLIC NYM");
        std::cout << armored << std::endl;
    }

    return 1;
}
}  // namespace opentxs
