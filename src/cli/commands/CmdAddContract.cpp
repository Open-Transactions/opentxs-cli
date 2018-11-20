// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdAddContract.hpp"

#include <opentxs/opentxs.hpp>

#define OT_METHOD "opentxs::CmdAddContract::"

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
        LogOutput(OT_METHOD)(__FUNCTION__)(
            ": Unknown nym ")(mynym)(".").Flush();

        return -1;
    }

    if (type.empty()) {
        LogOutput(OT_METHOD)(__FUNCTION__)(": Invalid type.").Flush();

        return -1;
    }

    if (value.empty()) {
        LogOutput(OT_METHOD)(__FUNCTION__)(": Invalid value.").Flush();

        return -1;
    }

    const auto currency = static_cast<proto::ContactItemType>(std::stoi(type));
    auto data = SwigWrap::Wallet_GetNym(mynym);

    if (false == data.Valid()) {
        LogOutput(OT_METHOD)(__FUNCTION__)(": Invalid nym ")(mynym)(".")
        .Flush();

        return -1;
    }

    const auto set = data.AddContract(value, currency, true, true);

    if (false == set) {
        LogOutput(OT_METHOD)(__FUNCTION__)(": Failed to set claim.").Flush();

        return -1;
    }

    LogOutput(OT_METHOD)(__FUNCTION__)(": ")(
        SwigWrap::DumpContactData(mynym))(".").Flush();

    return 1;
}
}  // namespace opentxs
