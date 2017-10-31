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

#include "CmdSignContract.hpp"

#include "CmdBase.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/SwigWrap.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdSignContract::CmdSignContract()
{
    command = "signcontract";
    args[0] = "--mynym <nym>";
    args[1] = "[--type <contracttype>]";
    category = catAdmin;
    help = "Add a signature to a contract and erase all others";
    usage = "If --type is specified, flatsign the contract using that type.";
}

CmdSignContract::~CmdSignContract()
{
}

int32_t CmdSignContract::runWithOptions()
{
    return run(getOption("mynym"), getOption("type"));
}

// Addsignature leaves all signatures in place and appends the new one
// Signcontract erases all signatures and replaces them with the new one
int32_t CmdSignContract::run(string mynym, string type)
{
    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    string contract = inputText("a contract to sign");
    if ("" == contract) {
        return -1;
    }

    string output = SwigWrap::SignContract(mynym, contract);
    if ("" == output) {
        if (!checkMandatory("type", type)) {
            return -1;
        }

        output = SwigWrap::FlatSign(mynym, contract, type);
        if ("" == output) {
            otOut << "Error: cannot sign contract.\n";
            return -1;
        }
    }

    dashLine();
    cout << "Signed:\n" << output << "\n";

    return 1;
}
