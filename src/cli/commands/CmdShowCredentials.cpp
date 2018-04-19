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

#include "CmdShowCredentials.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

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
    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    int32_t items = SwigWrap::GetNym_MasterCredentialCount(mynym);
    if (0 > items) {
        otOut << "Error: cannot load credential list item count.\n";
        return -1;
    }

    int32_t revokedItems = SwigWrap::GetNym_RevokedCredCount(mynym);
    if (0 > revokedItems) {
        otOut << "Error: cannot load revoked credential list item count.\n";
        return -1;
    }

    if (0 == items) {
        otOut << "The credential list is empty.\n";
    } else {
        otOut << "Idx     Credential ID\n"
                 "---------------------------\n";

        for (int32_t i = 0; i < items; i++) {
            string credential = SwigWrap::GetNym_MasterCredentialID(mynym, i);
            cout << i << ":      " << credential << "\n";

            int32_t subItems =
                SwigWrap::GetNym_ChildCredentialCount(mynym, credential);
            if (1 <= subItems) {
                otOut << "        ---------------------------\n"
                         "        Idx     Credential ID\n"
                         "        ---------------------------\n";

                for (int32_t j = 0; j < subItems; j++) {
                    string childCred = SwigWrap::GetNym_ChildCredentialID(
                        mynym, credential, j);
                    cout << "        " << j << ":      " << childCred << "\n";
                }
            }
        }
    }

    if (0 == revokedItems) {
        cout << "The revoked credential list is empty.\n";
    } else {
        otOut << "Idx     Revoked Credential ID\n"
                 "---------------------------\n";

        for (int32_t i = 0; i < revokedItems; i++) {
            string credential = SwigWrap::GetNym_RevokedCredID(mynym, i);
            cout << i << ":      " << credential << "\n";

            int32_t subItems =
                SwigWrap::GetNym_ChildCredentialCount(mynym, credential);
            if (1 <= subItems) {
                otOut << "        ---------------------------\n"
                         "        Idx     Revoked Credential ID\n"
                         "        ---------------------------\n";

                for (int32_t j = 0; j < subItems; j++) {
                    string childCred = SwigWrap::GetNym_ChildCredentialID(
                        mynym, credential, j);
                    cout << "        " << j << ":      " << childCred << "\n";
                }
            }
        }
    }

    return (0 == items + revokedItems) ? 0 : 1;
}
