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

#include "CmdPayDividend.hpp"

#include "CmdBase.hpp"

#include <opentxs/client/SwigWrap.hpp>

#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/OT.hpp>
#include <opentxs/client/OT_ME.hpp>
#include <opentxs/core/util/Common.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdPayDividend::CmdPayDividend()
{
    command = "paydividend";
    args[0] = "--myacct <account>";
    args[1] = "--hispurse <purse>";
    args[2] = "--amount <pershare>";
    args[3] = "[--memo <memoline>]";
    category = catMarkets;
    help = "Send dividend payout to all shareholders (in voucher form).";
}

CmdPayDividend::~CmdPayDividend() {}

int32_t CmdPayDividend::runWithOptions()
{
    return run(
        getOption("myacct"),
        getOption("hispurse"),
        getOption("amount"),
        getOption("memo"));
}

// Just like withdraw voucher...except instead of withdrawing a single voucher
// to yourself, it removes the total dividend payout from your account, and then
// divides it up amongst the shareholders, sending them EACH a voucher cheque in
// the amount of amount * number of shares owned.

int32_t CmdPayDividend::run(
    string myacct,
    string hispurse,
    string amount,
    string memo)
{
    if (!checkAccount("myacct", myacct)) {
        return -1;
    }

    if (!checkPurse("hispurse", hispurse)) {
        return -1;
    }

    int64_t value = checkAmount("amount", amount, myacct);
    if (OT_ERROR_AMOUNT == value) {
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

    string response = OT::App().API().OTME().pay_dividend(
        server, mynym, myacct, hispurse, memo, value);
    int32_t reply =
        responseReply(response, server, mynym, myacct, "pay_dividend");
    if (1 == reply) {
        return reply;
    }

    if (!OT::App().API().OTME().retrieve_account(server, mynym, myacct, true)) {
        otOut << "Error retrieving intermediary files for account.\n";
        return -1;
    }

    return 1;
}
