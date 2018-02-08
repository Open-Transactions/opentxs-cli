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

#include "CmdSendCash.hpp"

#include "CmdBase.hpp"
#include "CmdExportCash.hpp"
#include "CmdWithdrawCash.hpp"

#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/client/OT_ME.hpp>
#include <opentxs/client/SwigWrap.hpp>
#include <opentxs/core/util/Common.hpp>
#include <opentxs/core/Log.hpp>
#include <opentxs/OT.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdSendCash::CmdSendCash()
{
    command = "sendcash";
    args[0] = "[--server <server>]";
    args[1] = "[--mynym <nym>]";
    args[2] = "[--myacct <account>]";
    args[3] = "[--mypurse <purse>]";
    args[4] = "--hisnym <nym>";
    args[5] = "--amount <amount>";
    args[6] = "[--indices <indices|all>]";
    args[7] = "[--password <true|false>]";
    category = catOtherUsers;
    help = "Send cash from mypurse to recipient, withdraw if necessary.";
    usage = "Specify either myacct OR mypurse.\n"
            "When mypurse is specified server and mynym are mandatory.";
}

CmdSendCash::~CmdSendCash() {}

int32_t CmdSendCash::runWithOptions()
{
    return run(
        getOption("server"),
        getOption("mynym"),
        getOption("myacct"),
        getOption("mypurse"),
        getOption("hisnym"),
        getOption("amount"),
        getOption("indices"),
        getOption("password"));
}

int32_t CmdSendCash::run(
    string server,
    string mynym,
    string myacct,
    string mypurse,
    string hisnym,
    string amount,
    string indices,
    string password)
{
    if ("" != myacct) {
        if (!checkAccount("myacct", myacct)) {
            return -1;
        }

        // myacct specified: server and mynym are implied
        server = SwigWrap::GetAccountWallet_NotaryID(myacct);
        if ("" == server) {
            otOut << "Error: cannot determine server from myacct.\n";
            return -1;
        }

        mynym = SwigWrap::GetAccountWallet_NymID(myacct);
        if ("" == mynym) {
            otOut << "Error: cannot determine mynym from myacct.\n";
            return -1;
        }

        string assetType = getAccountAssetType(myacct);
        if ("" == assetType) {
            return -1;
        }

        if ("" != mypurse && mypurse != assetType) {
            otOut << "Error: myacct instrument definition does not match "
                     "mypurse.\n";
            return -1;
        }

        mypurse = assetType;
    } else {
        // we want either ONE OF myacct OR mypurse to be specified
        if (!checkMandatory("myacct or mypurse", mypurse)) {
            return -1;
        }

        if (!checkPurse("mypurse", mypurse)) {
            return -1;
        }

        // mypurse specified: server and mynym are mandatory
        if (!checkServer("server", server)) {
            return -1;
        }

        if (!checkNym("mynym", mynym)) {
            return -1;
        }
    }

    if (!checkNym("hisnym", hisnym)) {
        return -1;
    }

    if ("" != password && !checkFlag("password", password)) {
        return -1;
    }

    int64_t value = checkAmount("amount", amount, myacct);
    if (OT_ERROR_AMOUNT == value) {
        return -1;
    }

    // Below this point we can just try to pay it from the purse, and if unable
    // to, try to get the remaining funds from the account, IF that's available.

    string response = "";
    if (1 != sendCash(
                 response,
                 server,
                 mynym,
                 mypurse,
                 myacct,
                 hisnym,
                 amount,
                 indices,
                 password == "true")) {
        return -1;
    }

    cout << response << "\n";

    return 1;
}

int32_t CmdSendCash::sendCash(
    string& response,
    const string& server,
    const string& mynym,
    const string& assetType,
    const string& myacct,
    string& hisnym,
    const string& amount,
    string& indices,
    bool hasPassword) const
{
#if OT_CASH
    int64_t startAmount = "" == amount ? 0 : stoll(amount);

    // What we want to do from here is, see if we can send the cash purely using
    // cash we already have in the local purse. If so, we just package it up and
    // send it off using send_user_payment.
    //
    // But if we do NOT have the proper cash tokens in the local purse to send,
    // then we need to withdraw enough tokens until we do, and then try sending
    // again.

    int64_t remain = startAmount;
    if (!getPurseIndicesOrAmount(server, mynym, assetType, remain, indices)) {
        if ("" != indices) {
            otOut << "Error: invalid purse indices.\n";
            return -1;
        }

        // Not enough cash found in existing purse to match the amount
        CmdWithdrawCash cmd;
        if (1 != cmd.withdrawCash(myacct, remain)) {
            otOut << "Error: cannot withdraw cash.\n";
            return -1;
        }

        remain = startAmount;
        if (!getPurseIndicesOrAmount(
                server, mynym, assetType, remain, indices)) {
            otOut << "Error: cannot retrieve purse indices.\n";
            return -1;
        }
    }

    CmdExportCash cmd;
    string retainedCopy = "";
    string exportedCash = cmd.exportCash(
        server, mynym, assetType, hisnym, indices, hasPassword, retainedCopy);
    if ("" == exportedCash) {
        otOut << "Error: cannot export cash.\n";
        return -1;
    }

    response = OT::App().API().OTME().send_user_cash(
        server, mynym, hisnym, exportedCash, retainedCopy);
    if (1 != responseStatus(response)) {
        // cannot send cash so try to re-import into sender's purse
        if (!SwigWrap::Wallet_ImportPurse(
                server, assetType, mynym, retainedCopy)) {
            otOut << "Error: cannot send cash AND failed re-importing purse."
                  << "\nServer: " << server << "\nAsset Type: " << assetType
                  << "\nNym: " << mynym
                  << "\n\nPurse (SAVE THIS SOMEWHERE!):\n\n"
                  << retainedCopy << "\n";
            return -1;
        }

        // at least re-importing succeeeded
        otOut << "Error: cannot send cash.\n";
        return -1;
    }

    return 1;
#else
    return -1;
#endif  // OT_CASH
}

// If you pass the indices, this function returns true if those exact indices
// exist. In that case, this function will also set remain to the total.
//
// If, instead, you pass remain and a blank indices, this function will try to
// determine the indices that would create remain, if they were selected.

bool CmdSendCash::getPurseIndicesOrAmount(
    const string& server,
    const string& mynym,
    const string& assetType,
    int64_t& remain,
    string& indices) const
{
#if OT_CASH
    bool findAmountFromIndices = "" != indices && 0 == remain;
    bool findIndicesFromAmount = "" == indices && 0 != remain;
    if (!findAmountFromIndices && !findIndicesFromAmount) {
        otOut << "Error: invalid parameter combination.\n";
        return false;
    }

    string purse = SwigWrap::LoadPurse(server, assetType, mynym);
    if ("" == purse) {
        otOut << "Error: cannot load purse.\n";
        return false;
    }

    int32_t items = SwigWrap::Purse_Count(server, assetType, purse);
    if (0 > items) {
        otOut << "Error: cannot load purse item count.\n\n";
        return false;
    }

    if (0 == items) {
        otOut << "Error: the purse is empty.\n\n";
        return false;
    }

    for (int32_t i = 0; i < items; i++) {
        string token = SwigWrap::Purse_Peek(server, assetType, mynym, purse);
        if ("" == token) {
            otOut << "Error:cannot load token from purse.\n";
            return false;
        }

        purse = SwigWrap::Purse_Pop(server, assetType, mynym, purse);
        if ("" == purse) {
            otOut << "Error: cannot load updated purse.\n";
            return false;
        }

        int64_t denomination =
            SwigWrap::Token_GetDenomination(server, assetType, token);
        if (0 >= denomination) {
            otOut << "Error: cannot get token denomination.\n";
            return false;
        }

        time64_t validTo = SwigWrap::Token_GetValidTo(server, assetType, token);
        if (OT_TIME_ZERO > validTo) {
            otOut << "Error: cannot get token validTo.\n";
            return false;
        }

        time64_t time = SwigWrap::GetTime();
        if (OT_TIME_ZERO > time) {
            otOut << "Error: cannot get token time.\n";
            return false;
        }

        if (time > validTo) {
            otOut << "Skipping: token is expired.\n";
            continue;
        }

        if (findAmountFromIndices) {
            if ("all" == indices ||
                SwigWrap::NumList_VerifyQuery(indices, to_string(i))) {
                remain += denomination;
            }
            continue;
        }

        // TODO: There could be a denomination order that will cause this
        // function to fail, even though there is a denomination combination
        // that would make it succeeed. Example: try to find 6 when the
        // denominations are: 5, 2, 2, and 2. This will not succeed since it
        // will use the 5 first and then cannot satisfy the remaining 1 even
        // though the three 2's would satisfy the 6...

        if (denomination <= remain) {
            indices = SwigWrap::NumList_Add(indices, to_string(i));
            remain -= denomination;
            if (0 == remain) {
                return true;
            }
        }
    }

    return findAmountFromIndices ? true : false;
#else
    return false;
#endif  // OT_CASH
}
