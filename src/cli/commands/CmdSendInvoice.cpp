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

#include "CmdSendInvoice.hpp"

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdSendInvoice::CmdSendInvoice()
{
    command = "sendinvoice";
    args[0] = "--myacct <account>";
    args[1] = "[--hisnym <nym>]";
    args[2] = "--amount <amount>";
    args[3] = "[--memo <memoline>]";
    args[4] = "[--validfor <seconds>]";
    category = catOtherUsers;
    help = "Write an invoice and then send it to the recipient.";
    usage = "Use writeinvoice if you don't want to send it immediately.";
}

CmdSendInvoice::~CmdSendInvoice() {}

int32_t CmdSendInvoice::runWithOptions()
{
    return run(
        getOption("myacct"),
        getOption("hisnym"),
        getOption("amount"),
        getOption("memo"),
        getOption("validfor"));
}

int32_t CmdSendInvoice::run(
    string myacct,
    string hisnym,
    string amount,
    string memo,
    string validfor)
{
    string cheque = writeCheque(myacct, hisnym, amount, memo, validfor, true);
    if ("" == cheque) { return -1; }

    cout << cheque << "\n";

    return sendPayment(cheque, "", "send invoice");
}
