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

#include "CmdClearRecords.hpp"

#include "CmdBase.hpp"

#include <opentxs/client/SwigWrap.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdClearRecords::CmdClearRecords()
{
    command = "clearrecords";
    args[1] = "--myacct <account>";
    category = catMisc;
    help = "Clear all archived records and receipts.";
}

CmdClearRecords::~CmdClearRecords()
{
}

int32_t CmdClearRecords::runWithOptions()
{
    return run(getOption("myacct"));
}

int32_t CmdClearRecords::run(string myacct)
{
    if (!checkAccount("myacct", myacct)) {
        return -1;
    }

    string server = SwigWrap::GetAccountWallet_NotaryID(myacct);
    if ("" == server) {
        otOut << "Error: cannot determine server from myacct.\n";
        return -1;
    }

    string mynym = SwigWrap::GetAccountWallet_NymID(myacct);
    if ("" == mynym) {
        otOut << "Error: cannot determine mynym from myacct.\n";
        return -1;
    }

    // Recordbox for mynym contains the old payments (in and out.)
    otOut << "Clearing archived Nym-related records...\n";
    bool success = SwigWrap::ClearRecord(server, mynym, mynym, 0, true);

    dashLine();

    // Recordbox for myacct contains the old inbox receipts.
    otOut << "\nClearing archived Account-related records...\n";
    success |= SwigWrap::ClearRecord(server, mynym, myacct, 0, true);

    return success ? 1 : -1;
}
