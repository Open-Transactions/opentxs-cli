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

#include "CmdAcceptMoney.hpp"

#include "CmdBase.hpp"
#include "CmdBaseAccept.hpp"

#include <stdint.h>
#include <string>

using namespace opentxs;
using namespace std;

CmdAcceptMoney::CmdAcceptMoney()
{
    command = "acceptmoney";
    args[0] = "--myacct <account>";
    category = catAccounts;
    help = "Accept all incoming transfers and payments into myacct.";
}

CmdAcceptMoney::~CmdAcceptMoney()
{
}

int32_t CmdAcceptMoney::runWithOptions()
{
    return run(getOption("myacct"));
}

int32_t CmdAcceptMoney::run(string myacct)
{
    if (!checkAccount("myacct", myacct)) {
        return -1;
    }

    // FIX: these OR's should become AND's so we can detect any failure
    bool success = 0 <= acceptFromInbox(myacct, "all", typeTransfers);
    success |= 0 <= acceptFromPaymentbox(myacct, "all", "PURSE");
    success |= 0 <= acceptFromPaymentbox(myacct, "all", "CHEQUE");
    return success ? 1 : -1;
}
