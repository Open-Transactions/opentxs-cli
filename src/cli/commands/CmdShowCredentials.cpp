// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdShowCredentials.hpp"

#include <opentxs/opentxs.hpp>

#include <iostream>
#include <stdint.h>
#include <string>

#define OT_METHOD "opentxs::CmdShowCredentials::"

using namespace opentxs;
using namespace std;

CmdShowCredentials::CmdShowCredentials()
{
    command = "showcredentials";
    args[0] = "--mynym <nym>";
    category = catNyms;
    help = "Show mynym's credentials.";
}

CmdShowCredentials::~CmdShowCredentials() {}

int32_t CmdShowCredentials::runWithOptions() { return run(getOption("mynym")); }

int32_t CmdShowCredentials::run(string mynym)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    auto nym = Opentxs::Client().Wallet().Nym(Identifier::Factory(mynym));
    auto masterCredentialIDs = nym->GetMasterCredentialIDs();

    if (0 == masterCredentialIDs.size()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": The credential list is empty.")
            .Flush();
    } else {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Idx     Credential ID\n"
                                           "---------------------------")
            .Flush();

        auto i = 0;
        for (auto masterCredentialID : masterCredentialIDs) {
            auto masterCredential = masterCredentialID->str();
            cout << i++ << ":      " << masterCredential << "\n";

            auto childCredentialIDs =
                nym->GetChildCredentialIDs(masterCredentialID->str());
            if (childCredentialIDs.size()) {
                LogNormal(OT_METHOD)(__FUNCTION__)(
                    ":       ---------------------------"
                    "        Idx     Credential ID\n"
                    "        ---------------------------")
                    .Flush();

                auto j = 0;
                for (auto childCredentialID : childCredentialIDs) {
                    cout << "        " << j++ << ":      "
                         << childCredentialID->str() << "\n";
                }
            }
        }
    }

    auto revokedCredentialIDs = nym->GetRevokedCredentialIDs();

    if (0 == revokedCredentialIDs.size()) {
        cout << "The revoked credential list is empty.\n";
    } else {
        LogNormal(OT_METHOD)(__FUNCTION__)(":Idx     Revoked Credential ID"
                                           "---------------------------")
            .Flush();

        auto i = 0;
        for (auto revokedCredentialID : revokedCredentialIDs) {
            auto revokedCredential = revokedCredentialID->str();
            cout << i++ << ":      " << revokedCredential << "\n";

            auto childCredentialIDs =
                nym->GetChildCredentialIDs(revokedCredentialID->str());
            if (childCredentialIDs.size()) {
                LogNormal(OT_METHOD)(__FUNCTION__)(
                    ":       ---------------------------"
                    "        Idx     Revoked Credential ID"
                    "        ---------------------------")
                    .Flush();

                auto j = 0;
                for (auto childCredentialID : childCredentialIDs) {
                    cout << "        " << j++ << ":      "
                         << childCredentialID->str() << "\n";
                }
            }
        }
    }

    return (0 == masterCredentialIDs.size() + revokedCredentialIDs.size()) ? 0
                                                                           : 1;
}
