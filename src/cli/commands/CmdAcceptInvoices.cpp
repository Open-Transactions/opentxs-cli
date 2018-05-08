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

#include <cstdint>
#include <string>

using namespace opentxs;
using namespace std;

CmdAcceptInvoices::CmdAcceptInvoices()
{
    command = "acceptinvoices";
    args[0] = "--myacct <account>";
    args[1] = "[--server <server>]";
    args[2] = "[--indices <indices|all>]";
    category = catAccounts;
    help = "Pay all invoices in myacct's Nym's payments inbox on Server."
    " (Confused yet? New API fixes this kind of problem).";
    usage = "Omitting --indices is the same as specifying --indices all. "
    "And FYI, it uses MyAcct's server if one is not provided.";
}

CmdAcceptInvoices::~CmdAcceptInvoices()
{
}

std::int32_t CmdAcceptInvoices::runWithOptions()
{
    return run(getOption("server"), getOption("myacct"), getOption("indices"));
}

std::int32_t CmdAcceptInvoices::run(
    string server,
    string myacct,
    string indices)
{
    if (!checkAccount("myacct", myacct)) {
        return -1;
    }

    if (!checkServer("server", server)) {
        server = SwigWrap::GetAccountWallet_NotaryID(myacct);
    }
    if (!checkServer("server", server)) {
        return -1;
    }
    string & transport_notary = server;

    if ("" != indices && !checkIndices("indices", indices)) {
        return -1;
    }

    return acceptFromPaymentbox(transport_notary, myacct, indices, "INVOICE");
}
