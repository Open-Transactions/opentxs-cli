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

#include "CmdWithdrawVoucher.hpp"

#include "CmdBase.hpp"

#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/api/OT.hpp>
#include <opentxs/client/MadeEasy.hpp>
#include <opentxs/client/OT_ME.hpp>
#include <opentxs/client/SwigWrap.hpp>
#include <opentxs/core/util/Common.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdWithdrawVoucher::CmdWithdrawVoucher()
{
    command = "withdrawvoucher";
    args[0] = "--myacct <account>";
    args[1] = "--hisnym <nym>";
    args[2] = "--amount <amount>";
    args[3] = "[--memo <memoline>]";
    category = catInstruments;
    help = "Withdraw from myacct as a voucher (cashier's cheque).";
    usage = "Use sendvoucher if you want to send it immediately.";
}

CmdWithdrawVoucher::~CmdWithdrawVoucher()
{
}

int32_t CmdWithdrawVoucher::runWithOptions()
{
    return run(getOption("myacct"), getOption("hisnym"), getOption("amount"),
               getOption("memo"));
}

int32_t CmdWithdrawVoucher::run(string myacct, string hisnym, string amount,
                                string memo)
{
    string voucher;
    if (1 > run(myacct, hisnym, amount, memo, voucher)) {
        return -1;
    }

    cout << voucher << "\n";

    return 1;
}

int32_t CmdWithdrawVoucher::run(string myacct, string hisnym, string amount,
                                string memo, string& voucher)
{
    if (!checkAccount("myacct", myacct)) {
        return -1;
    }

    if (!checkNym("hisnym", hisnym, false)) {
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

    // make sure we can access the public key before trying to write a voucher
    if ("" == OT::App().API().ME().load_or_retrieve_encrypt_key(server, mynym, hisnym)) {
        otOut << "Error: cannot load public key for hisnym.\n";
        return -1;
    }


    string response =
        OT_ME::It().withdraw_voucher(server, mynym, myacct, hisnym, memo, value);
    int32_t reply =
        responseReply(response, server, mynym, myacct, "withdraw_voucher");
    if (1 != reply) {
        return reply;
    }

    string ledger = SwigWrap::Message_GetLedger(response);
    if ("" == ledger) {
        otOut << "Error: cannot load ledger.\n";
        return -1;
    }

    string tx = SwigWrap::Ledger_GetTransactionByIndex(server, mynym, myacct,
                                                         ledger, 0);
    if ("" == tx) {
        otOut << "Error: cannot retrieve transaction.\n";
        return -1;
    }

    voucher = SwigWrap::Transaction_GetVoucher(server, mynym, myacct, tx);
    if ("" == voucher) {
        otOut << "Error: cannot load voucher.\n";
        return -1;
    }

    // Save a copy in my own outpayments box. I don't want to lose this
    // voucher since it uses one of my own transaction numbers. (If I later
    // send the voucher to someone, OT is smart enough to remove the first
    // copy from outpayments, when adding the second copy.)
    //
    // Notice how I can send an instrument to myself. This doesn't actually
    // send anything -- it just puts a copy into my outpayments box for
    // safe-keeping.
    OT_ME::It().send_user_payment(server, mynym, mynym, voucher);

    if (!OT::App().API().ME().retrieve_account(server, mynym, myacct, true)) {
        otOut << "Error retrieving intermediary files for account.\n";
        return -1;
    }

    return 1;
}
