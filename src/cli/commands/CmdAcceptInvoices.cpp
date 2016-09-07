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

#include "CmdAcceptInvoices.hpp"

#include "CmdBase.hpp"

#include <stdint.h>
#include <string>

using namespace opentxs;
using namespace std;

CmdAcceptInvoices::CmdAcceptInvoices()
{
    command = "acceptinvoices";
    args[0] = "--myacct <account>";
    args[1] = "[--indices <indices|all>]";
    category = catAccounts;
    help = "Pay all invoices in myacct's payments inbox.";
    usage = "Omitting --indices is the same as specifying --indices all.";
}

CmdAcceptInvoices::~CmdAcceptInvoices()
{
}

int32_t CmdAcceptInvoices::runWithOptions()
{
    return run(getOption("myacct"), getOption("indices"));
}

int32_t CmdAcceptInvoices::run(string myacct, string indices)
{
    if (!checkAccount("myacct", myacct)) {
        return -1;
    }

    if ("" != indices && !checkIndices("indices", indices)) {
        return -1;
    }

    return acceptFromPaymentbox(myacct, indices, "INVOICE");
}
