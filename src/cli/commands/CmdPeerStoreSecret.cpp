// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdPeerStoreSecret.hpp"

#include <opentxs/opentxs.hpp>

#define OT_METHOD "opentxs::CmdPeerStoreSecret"

namespace opentxs
{

CmdPeerStoreSecret::CmdPeerStoreSecret()
{
    command = "peerstoresecret";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--hisnym <recipient>";
    category = catMisc;
    help = "Request a nym to store a BIP-39 seed on behalf of the requestor";
}

CmdPeerStoreSecret::~CmdPeerStoreSecret() {}

std::int32_t CmdPeerStoreSecret::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("hisnym"));
}

std::int32_t CmdPeerStoreSecret::run(
    std::string server,
    std::string mynym,
    std::string hisnym)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    if (!checkNym("hisnym", hisnym)) { return -1; }

    const std::string primary = inputText("Word list");

    if (0 == primary.size()) { return -1; }

    const std::string secondary = inputText("Passphrase");

    auto task = Opentxs::Client().OTX().InitiateStoreSecret(
        identifier::Nym::Factory(mynym),
        identifier::Server::Factory(server),
        identifier::Nym::Factory(hisnym),
        proto::SecretType(1),
        primary,
        secondary);

    const auto result = std::get<1>(task).get();

    if (false == CmdBase::GetResultSuccess(result)) {
        LogOutput(OT_METHOD)(__FUNCTION__)(": Failed to register contract")
            .Flush();
        return -1;
    }

    return 1;
}
}  // namespace opentxs
