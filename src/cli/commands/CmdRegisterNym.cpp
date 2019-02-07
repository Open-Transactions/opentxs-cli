// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdRegisterNym.hpp"

#include <opentxs/opentxs.hpp>

#define OT_METHOD "opentxs::CmdRegisterNym"

namespace opentxs
{

CmdRegisterNym::CmdRegisterNym()
{
    command = "registernym";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "[--publish <true|FALSE>]";
    args[3] = "[--primary <true|FALSE>]";
    category = catAdmin;
    help = "Register mynym onto an OT server.";
    usage = "Use --publish to set the server as mynym's preferred server and "
            "--primary to force it to replace the current one.";
}

std::int32_t CmdRegisterNym::runWithOptions()
{
    return run(
        getOption("server"),
        getOption("mynym"),
        getOption("publish"),
        getOption("primary"));
}

std::int32_t CmdRegisterNym::run(
    std::string server,
    std::string mynym,
    std::string publish,
    std::string primary)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    bool shouldPublish{false};
    if ("" != publish && !checkBoolean("publish", publish)) {
        return -1;
    } else {
        shouldPublish = publish == "true";
    }
    if (!checkNym("mynym", mynym)) { return -1; }
    bool isPrimary{false};
    if ("" != primary && !checkBoolean("primary", primary)) {
        return -1;
    } else {
        isPrimary = primary == "true";
    }
    if (!shouldPublish && isPrimary) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Can't make the server primary "
                                           "if it isn't published.")
            .Flush();

        return -1;
    }

    auto& OTX = Opentxs::Client().OTX();

    auto task = OTX.RegisterNymPublic(
        Identifier::Factory(mynym),
        Identifier::Factory(server),
        shouldPublish,
        isPrimary);

    const auto result = std::get<1>(task).get();
    
    const auto success = CmdBase::GetResultSuccess(result);
    if (false == success) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Nym not registered.").Flush();
        return -1;
    }

    LogNormal(OT_METHOD)(__FUNCTION__)(": Nym registered successfully. ")
        .Flush();

    return 0;
}
}  // namespace opentxs
