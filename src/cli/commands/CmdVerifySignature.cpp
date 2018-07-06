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

#include "CmdVerifySignature.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdVerifySignature::CmdVerifySignature()
{
    command = "verifysignature";
    args[0] = "--hisnym <nym>";
    category = catAdmin;
    help = "Verify hisnym's signature on a pasted contract.";
}

CmdVerifySignature::~CmdVerifySignature() {}

int32_t CmdVerifySignature::runWithOptions()
{
    return run(getOption("hisnym"));
}

int32_t CmdVerifySignature::run(string hisnym)
{
    if (!checkNym("hisnym", hisnym)) { return -1; }

    string contract = inputText("the contract to verify");
    if ("" == contract) { return -1; }

    if (!SwigWrap::VerifySignature(hisnym, contract)) {
        otOut << "Error: cannot verify signature.\n";
        return -1;
    }

    cout << "Signature was verified!\n";

    return 1;
}
