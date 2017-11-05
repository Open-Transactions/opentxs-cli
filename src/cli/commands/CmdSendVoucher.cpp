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

#include "CmdSendVoucher.hpp"

#include "CmdBase.hpp"
#include "CmdWithdrawVoucher.hpp"

#include <opentxs/client/SwigWrap.hpp>
#include <opentxs/core/util/Common.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdSendVoucher::CmdSendVoucher()
{
    command = "sendvoucher";
    args[0] = "--myacct <account>";
    args[1] = "--hisnym <nym>";
    args[2] = "--amount <amount>";
    args[3] = "[--memo <memoline>]";
    category = catOtherUsers;
    help = "Withdraw a voucher and then send it to the recipient.";
    usage = "Use withdrawvoucher if you don't want to send it immediately.";
}

CmdSendVoucher::~CmdSendVoucher()
{
}

int32_t CmdSendVoucher::runWithOptions()
{
    return run(getOption("myacct"), getOption("hisnym"), getOption("amount"),
               getOption("memo"));
}

int32_t CmdSendVoucher::run(string myacct, string hisnym, string amount,
                            string memo)
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

    string voucher;
    CmdWithdrawVoucher withdrawVoucher;
    if (1 > withdrawVoucher.run(myacct, hisnym, amount, memo, voucher)) {
        return -1;
    }

    cout << voucher << "\n";

    string sender = SwigWrap::GetAccountWallet_NymID(myacct);
    if ("" == sender) {
        otOut << "Error: cannot get sender from myacct.\n";
        return -1;
    }

    return sendPayment(voucher, sender, "send voucher");
}
