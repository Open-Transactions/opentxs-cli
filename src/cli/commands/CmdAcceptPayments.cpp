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

#include "CmdAcceptPayments.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <string>

using namespace opentxs;
using namespace std;

CmdAcceptPayments::CmdAcceptPayments()
{
    command = "acceptpayments";
    args[0] = "--myacct <account>";
    args[1] = "[--indices <indices|all>]";
    category = catAccounts;
    help = "Accept all incoming payments in myacct's payments inbox.";
    usage = "Omitting --indices is the same as specifying --indices all.";
}

CmdAcceptPayments::~CmdAcceptPayments() {}

int32_t CmdAcceptPayments::runWithOptions()
{
    return run(getOption("myacct"), getOption("indices"));
}

int32_t CmdAcceptPayments::run(string myacct, string indices)
{
    if (!checkAccount("myacct", myacct)) {
        return -1;
    }

    if ("" != indices && !checkIndices("indices", indices)) {
        return -1;
    }

    // Note: Do NOT process invoices.
    // FIX: this OR should become AND so we can detect any failure
    bool success = 0 <= acceptFromPaymentbox(myacct, indices, "PURSE");
    success |= 0 <= acceptFromPaymentbox(myacct, indices, "CHEQUE");
    return success ? 1 : -1;
}
