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

#include "CmdVerifyReceipt.hpp"

#include "CmdBase.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/OTAPI_Wrap.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdVerifyReceipt::CmdVerifyReceipt()
{
    command = "verifyreceipt";
    args[0] = "--server <server>";
    args[1] = "--myacct <account>";
    category = catAccounts;
    help = "Verify your intermediary files against last signed receipt.";
}

CmdVerifyReceipt::~CmdVerifyReceipt()
{
}

int32_t CmdVerifyReceipt::runWithOptions()
{
    return run(getOption("server"), getOption("myacct"));
}

int32_t CmdVerifyReceipt::run(string server, string myacct)
{
    if (!checkServer("server", server)) {
        return -1;
    }

    if (!checkAccount("myacct", myacct)) {
        return -1;
    }

    string mynym = OTAPI_Wrap::GetAccountWallet_NymID(myacct);
    if ("" == mynym) {
        otOut << "Error: cannot determine mynym from myacct.\n";
        return -1;
    }

    if (!OTAPI_Wrap::VerifyAccountReceipt(server, mynym, myacct)) {
        otOut << "Error: cannot verify recepit.\n";
        return -1;
    }
    return 1;
}
