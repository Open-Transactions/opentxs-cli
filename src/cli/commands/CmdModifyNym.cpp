// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdModifyNym.hpp"

#include <opentxs/opentxs.hpp>

#define OT_METHOD "opentxs::CmdModifyNym::"

namespace opentxs
{
CmdModifyNym::CmdModifyNym()
{
    command = "modifynym";
    args[0] = "--mynym <nym>";
    args[1] = "--individual <label>";
    args[2] = "--server <label>";
    args[3] = "--opentxs <server id>";
    args[4] = "--core <payment code>";
    args[5] = "--testcore <payment code>";
    args[6] = "--cash <payment code>";
    args[7] = "--testcash <payment code>";
    category = catWallet;
    help = "Edit a nym's contact credential data.";
}

std::int32_t CmdModifyNym::runWithOptions()
{
    return run(
        getOption("mynym"),
        getOption("individual"),
        getOption("server"),
        getOption("opentxs"),
        getOption("core"),
        getOption("cash"),
        getOption("testcore"),
        getOption("testcash"));
}

std::int32_t CmdModifyNym::run(
    std::string mynym,
    const std::string& individual,
    const std::string& server,
    const std::string& opentxs,
    const std::string& core,
    const std::string& cash,
    const std::string& testCore,
    const std::string& testCash)
{
    if (false == checkNym("mynym", mynym)) {
        LogOutput(OT_METHOD)(__FUNCTION__)(
            ": Unknown nym ")(mynym)(".").Flush();

        return -1;
    }

    auto data = SwigWrap::Wallet_GetNym(mynym);

    if (false == data.Valid()) {
        LogOutput(OT_METHOD)(__FUNCTION__)(
            ": Invalid nym ")(mynym)(".").Flush();

        return -1;
    }

    if (false == individual.empty()) {
        data.SetScope(proto::CITEMTYPE_INDIVIDUAL, individual, true);
    }

    if (false == server.empty()) {
        data.SetScope(proto::CITEMTYPE_SERVER, server, true);
    }

    if (false == opentxs.empty()) { data.AddPreferredOTServer(opentxs, true); }

    if (false == core.empty()) {
        data.AddPaymentCode(core, proto::CITEMTYPE_BTC, true, true);
    }

    if (false == testCore.empty()) {
        data.AddPaymentCode(testCore, proto::CITEMTYPE_TNBTC, true, true);
    }

    if (false == cash.empty()) {
        data.AddPaymentCode(cash, proto::CITEMTYPE_BCH, true, true);
    }

    if (false == testCash.empty()) {
        data.AddPaymentCode(testCash, proto::CITEMTYPE_TNBCH, true, true);
    }

    LogOutput(OT_METHOD)(__FUNCTION__)(": ")(
        SwigWrap::DumpContactData(mynym))(".").Flush();

    return 1;
}
}  // namespace opentxs
