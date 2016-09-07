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

#include "CmdAcceptAll.hpp"

#include "CmdBase.hpp"
#include "CmdBaseAccept.hpp"

#include <stdint.h>
#include <string>

using namespace opentxs;
using namespace std;

CmdAcceptAll::CmdAcceptAll()
{
    command = "acceptall";
    args[0] = "--myacct <account>";
    category = catAccounts;
    help = "Accept all incoming transfers, receipts, payments, invoices.";
    usage = nullptr;
}

CmdAcceptAll::~CmdAcceptAll()
{
}

int32_t CmdAcceptAll::runWithOptions()
{
    return run(getOption("myacct"));
}

int32_t CmdAcceptAll::run(string myacct)
{
    if (!checkAccount("myacct", myacct)) {
        return -1;
    }

    //  Incoming transfers and receipts (asset account inbox.)
    bool success = 0 <= acceptFromInbox(myacct, "all", typeBoth);

    // Incoming payments -- cheques, purses, vouchers
    success &= 0 <= acceptFromPaymentbox(myacct, "all", "PURSE");
    success &= 0 <= acceptFromPaymentbox(myacct, "all", "CHEQUE");

    // Invoices LAST
    // So the MOST money is in the account before it starts paying out.
    success &= 0 <= acceptFromPaymentbox(myacct, "all", "INVOICE");

    return success ? 1 : -1;
}
