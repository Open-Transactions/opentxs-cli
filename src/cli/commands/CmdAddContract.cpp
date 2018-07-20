// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdAddContract.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{
CmdAddContract::CmdAddContract()
{
    command = "addcontract";
    args[0] = "--mynym <nym>";
    args[1] = "--type <currency>";
    args[2] = "--value <instrument definition id>";
    category = catWallet;
    help = "Edit a nym's contact credential data.";
}

std::int32_t CmdAddContract::runWithOptions()
{
    return run(getOption("mynym"), getOption("type"), getOption("value"));
}

std::int32_t CmdAddContract::run(
    std::string mynym,
    const std::string& type,
    const std::string& value)
{
    if (false == checkNym("mynym", mynym)) {
        otErr << "Unknown nym " << mynym << std::endl;

        return -1;
    }

    if (type.empty()) {
        otErr << "Invalid type" << std::endl;

        return -1;
    }

    if (value.empty()) {
        otErr << "Invalid value" << std::endl;

        return -1;
    }

    const auto currency = static_cast<proto::ContactItemType>(std::stoi(type));
    auto data = SwigWrap::Wallet_GetNym(mynym);

    if (false == data.Valid()) {
        otErr << "Invalid nym " << mynym << std::endl;

        return -1;
    }

    const auto set = data.AddContract(value, currency, true, true);

    if (false == set) {
        otErr << "Failed to set claim" << std::endl;

        return -1;
    }

    otErr << SwigWrap::DumpContactData(mynym) << std::endl;

    return 1;
}
}  // namespace opentxs
