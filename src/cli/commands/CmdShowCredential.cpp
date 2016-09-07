/************************************************************
 *
 *                 OPEN TRANSACTIONS
 *
 *       Financial Cryptography and Digital Cash
 *       Library, Protocol, API, Server, CLI, GUI
 *
 *       -- Anonymous Numbered Accounts.
 *       -- Untraceable Digital Cash.
 *       -- Triple-Signed Receipts.
 *       -- Cheques, Vouchers, Transfers, Inboxes.
 *       -- Basket Currencies, Markets, Payment Plans.
 *       -- Signed, XML, Ricardian-style Contracts.
 *       -- Scripted smart contracts.
 *
 *  EMAIL:
 *  fellowtraveler@opentransactions.org
 *
 *  WEBSITE:
 *  http://www.opentransactions.org/
 *
 *  -----------------------------------------------------
 *
 *   LICENSE:
 *   This Source Code Form is subject to the terms of the
 *   Mozilla Public License, v. 2.0. If a copy of the MPL
 *   was not distributed with this file, You can obtain one
 *   at http://mozilla.org/MPL/2.0/.
 *
 *   DISCLAIMER:
 *   This program is distributed in the hope that it will
 *   be useful, but WITHOUT ANY WARRANTY; without even the
 *   implied warranty of MERCHANTABILITY or FITNESS FOR A
 *   PARTICULAR PURPOSE.  See the Mozilla Public License
 *   for more details.
 *
 ************************************************************/

#include "CmdShowCredential.hpp"

#include "CmdBase.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/OTAPI_Wrap.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

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

CmdShowCredential::~CmdShowCredential()
{
}

int32_t CmdShowCredential::runWithOptions()
{
    return run(getOption("mynym"), getOption("id"));
}

int32_t CmdShowCredential::run(string mynym, string id)
{
    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    if (!checkMandatory("id", id)) {
        return -1;
    }

    string credential = OTAPI_Wrap::GetNym_MasterCredentialContents(mynym, id);
    if ("" != credential) {
        cout << "Master Credential contents:\n" << credential << "\n";
        return 1;
    }

    credential = OTAPI_Wrap::GetNym_RevokedCredContents(mynym, id);
    if ("" != credential) {
        cout << "Revoked Credential contents:\n" << credential << "\n";
        return 1;
    }

    // It MUST be a credential by this point.

    string master = findMaster(mynym, id);
    if ("" != master) {
        credential =
            OTAPI_Wrap::GetNym_ChildCredentialContents(mynym, master, id);
        if ("" == credential) {
            otOut << "Error: cannot load sub-credential.\n";
            return -1;
        }

        cout << "Credential contents:\n" << credential << "\n";
        return 1;
    }

    master = findRevoked(mynym, id);
    if ("" == master) {
        otOut << "Error: cannot find credential.\n";
        return -1;
    }

    credential = OTAPI_Wrap::GetNym_ChildCredentialContents(mynym, master, id);
    if ("" == credential) {
        otOut << "Error: cannot load sub-credential.\n";
        return -1;
    }

    cout << "Revoked Credential contents:\n" << credential << "\n";
    return 1;
}

string CmdShowCredential::findMaster(const string& mynym, const string& subID)
{
    int32_t items = OTAPI_Wrap::GetNym_MasterCredentialCount(mynym);
    if (0 >= items) {
        return "";
    }

    for (int32_t i = 0; i < items; i++) {
        string id = OTAPI_Wrap::GetNym_MasterCredentialID(mynym, i);
        int32_t subItems = OTAPI_Wrap::GetNym_ChildCredentialCount(mynym, id);
        for (int32_t j = 0; j < subItems; j++) {
            if (subID == OTAPI_Wrap::GetNym_ChildCredentialID(mynym, id, j)) {
                return id;
            }
        }
    }

    return "";
}

string CmdShowCredential::findRevoked(const string& mynym, const string& subID)
{
    int32_t items = OTAPI_Wrap::GetNym_RevokedCredCount(mynym);
    if (0 >= items) {
        return "";
    }

    for (int32_t i = 0; i < items; i++) {
        string id = OTAPI_Wrap::GetNym_RevokedCredID(mynym, i);
        int32_t subItems = OTAPI_Wrap::GetNym_ChildCredentialCount(mynym, id);
        for (int32_t j = 0; j < subItems; j++) {
            if (subID == OTAPI_Wrap::GetNym_ChildCredentialID(mynym, id, j)) {
                return id;
            }
        }
    }

    return "";
}
