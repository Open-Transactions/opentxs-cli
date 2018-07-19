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

#include "CmdWriteCheque.hpp"

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdWriteCheque::CmdWriteCheque()
{
    command = "writecheque";
    args[0] = "--myacct <account>";
    args[1] = "[--hisnym <nym>]";
    args[2] = "--amount <amount>";
    args[3] = "[--memo <memoline>]";
    args[4] = "[--validfor <seconds>]";
    category = catInstruments;
    help = "Write a cheque and print it out to the screen.";
    usage = "Use sendcheque if you want to write AND send the cheque.";
}

CmdWriteCheque::~CmdWriteCheque() {}

int32_t CmdWriteCheque::runWithOptions()
{
    return run(
        getOption("myacct"),
        getOption("hisnym"),
        getOption("amount"),
        getOption("memo"),
        getOption("validfor"));
}

int32_t CmdWriteCheque::run(
    string myacct,
    string hisnym,
    string amount,
    string memo,
    string validfor)
{
    string cheque = writeCheque(myacct, hisnym, amount, memo, validfor, false);
    if ("" == cheque) { return -1; }

    cout << cheque << "\n";

    return 1;
}
