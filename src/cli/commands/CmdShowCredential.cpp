// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdShowCredential.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

#define OT_METHOD "opentxs::CmdShowCredential::"

using namespace opentxs;
using namespace std;

CmdShowCredential::CmdShowCredential()
{
    command = "showcredential";
    args[0] = "--mynym <nym>";
    args[1] = "--id <credentialid>";
    category = catNyms;
    help = "Show a specific credential in detail.";
}

CmdShowCredential::~CmdShowCredential() {}

int32_t CmdShowCredential::runWithOptions()
{
    return run(getOption("mynym"), getOption("id"));
}

int32_t CmdShowCredential::run(string mynym, string id)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    if (!checkMandatory("id", id)) { return -1; }

    string credential = SwigWrap::GetNym_MasterCredentialContents(mynym, id);
    if ("" != credential) {
        cout << "Master Credential contents:\n" << credential << "\n";
        return 1;
    }

    credential = SwigWrap::GetNym_RevokedCredContents(mynym, id);
    if ("" != credential) {
        cout << "Revoked Credential contents:\n" << credential << "\n";
        return 1;
    }

    // It MUST be a credential by this point.

    string master = findMaster(mynym, id);
    if ("" != master) {
        credential =
            SwigWrap::GetNym_ChildCredentialContents(mynym, master, id);
        if ("" == credential) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                " : Error: cannot load sub-credential.")
                .Flush();
            return -1;
        }

        cout << "Credential contents:\n" << credential << "\n";
        return 1;
    }

    master = findRevoked(mynym, id);
    if ("" == master) {
        LogNormal(OT_METHOD)(__FUNCTION__)(" : Error: cannot find credential.")
            .Flush();
        return -1;
    }

    credential = SwigWrap::GetNym_ChildCredentialContents(mynym, master, id);
    if ("" == credential) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            " : Error: cannot load sub-credential.")
            .Flush();
        return -1;
    }

    cout << "Revoked Credential contents:\n" << credential << "\n";
    return 1;
}

string CmdShowCredential::findMaster(const string& mynym, const string& subID)
{
    auto nym = Opentxs::Client().Wallet().Nym(Identifier::Factory(mynym));
    auto masterCredentialIDs = nym->GetMasterCredentialIDs();
    for (auto masterCredentialID : masterCredentialIDs) {
        auto childCredentialIDs =
            nym->GetChildCredentialIDs(masterCredentialID->str());
        for (auto childCredentialID : childCredentialIDs) {
            if (subID == childCredentialID->str()) {
                return masterCredentialID->str();
            }
        }
    }

    return "";
}

string CmdShowCredential::findRevoked(const string& mynym, const string& subID)
{
    auto nym = Opentxs::Client().Wallet().Nym(Identifier::Factory(mynym));
    auto revokedCredentialIDs = nym->GetRevokedCredentialIDs();
    for (auto revokedCredentialID : revokedCredentialIDs) {
        auto childCredentialIDs =
            nym->GetChildCredentialIDs(revokedCredentialID->str());
        for (auto childCredentialID : childCredentialIDs) {
            if (subID == childCredentialID->str()) {
                return revokedCredentialID->str();
            }
        }
    }

    return "";
}
