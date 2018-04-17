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

#include "CmdTransfer.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdTransfer::CmdTransfer()
{
    command = "transfer";
    args[0] = "--myacct <account>";
    args[1] = "--hisacct <account>";
    args[2] = "--amount <amount>";
    args[3] = "[--memo <memoline>]";
    category = catAccounts;
    help = "Send a transfer from myacct to hisacct.";
}

CmdTransfer::~CmdTransfer() {}

int32_t CmdTransfer::runWithOptions()
{
    return run(
        getOption("myacct"),
        getOption("hisacct"),
        getOption("amount"),
        getOption("memo"));
}

int32_t CmdTransfer::run(
    string myacct,
    string hisacct,
    string amount,
    string memo)
{

    if (!checkAccount("myacct", myacct)) {
        return -1;
    }

    if (!checkAccount("hisacct", hisacct)) {
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

    string hisServer = SwigWrap::GetAccountWallet_NotaryID(hisacct);
    if (hisServer != server) {
        otOut << "Error: myacct and hisacct are on different servers.\n";
        return -1;
    }

    if ("" == hisServer) {
        otOut << "Assuming hisaccount is o5n the same server as myacct.\n";
    }

    opentxs::TransactionNumber notUsed{0};
    std::string response;
    {
        rLock lock (api_lock_);
        response = OT::App()
                          .API()
                          .ServerAction()
                          .SendTransfer(
                              Identifier(mynym),
                              Identifier(server),
                              Identifier(myacct),
                              Identifier(hisacct),
                              value,
                              memo)
                          ->Run();
    }
    int32_t reply =
        responseReply(response, server, mynym, myacct, "send_transfer");
    if (1 != reply) {
        return reply;
    }

    {
        rLock lock(api_lock_);
        if (!OT::App().API().ServerAction().DownloadAccount(
                Identifier(mynym), Identifier(server), Identifier(myacct), true)) {
            otOut << "Error retrieving intermediary files for account.\n";
            return -1;
        }
    }
    
    return 1;
}
