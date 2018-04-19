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

#include "CmdPasswordDecrypt.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdPasswordDecrypt::CmdPasswordDecrypt()
{
    command = "passworddecrypt";
    category = catAdmin;
    help = "Password-decrypt a ciphertext using a symmetric key.";
}

CmdPasswordDecrypt::~CmdPasswordDecrypt() {}

int32_t CmdPasswordDecrypt::runWithOptions() { return run(); }

int32_t CmdPasswordDecrypt::run()
{
    string key = inputText("a symmetric key");
    if ("" == key) {
        return -1;
    }

    string input = inputText("the encrypted text");
    if ("" == input) {
        return -1;
    }

    string encrypted = SwigWrap::SymmetricDecrypt(key, input);
    if ("" == encrypted) {
        otOut << "Error: cannot decrypt encrypted text.\n";
        return -1;
    }

    cout << encrypted << "\n";

    return 1;
}
