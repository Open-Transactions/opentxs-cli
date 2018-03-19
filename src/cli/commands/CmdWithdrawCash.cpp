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

#include "CmdWithdrawCash.hpp"

#include "CmdBase.hpp"

#include <opentxs/api/client/ServerAction.hpp>
#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/client/ServerAction.hpp>
#include <opentxs/client/SwigWrap.hpp>
#include <opentxs/core/util/Common.hpp>
#include <opentxs/core/Identifier.hpp>
#include <opentxs/core/Log.hpp>
#include <opentxs/OT.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdWithdrawCash::CmdWithdrawCash()
{
    command = "withdraw";
    args[0] = "--myacct <account>";
    args[1] = "--amount <amount>";
    category = catInstruments;
    help = "Withdraw from myacct as cash into local purse.";
}

CmdWithdrawCash::~CmdWithdrawCash() {}

int32_t CmdWithdrawCash::runWithOptions()
{
    return run(getOption("myacct"), getOption("amount"));
}

int32_t CmdWithdrawCash::run(string myacct, string amount)
{
    if (!checkAccount("myacct", myacct)) {
        return -1;
    }

    int64_t value = checkAmount("amount", amount, myacct);
    if (OT_ERROR_AMOUNT == value) {
        return -1;
    }

    return withdrawCash(myacct, value);
}

int32_t CmdWithdrawCash::withdrawCash(const string& myacct, int64_t amount)
    const
{
#if OT_CASH
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

    string assetType = getAccountAssetType(myacct);
    if ("" == assetType) {
        return -1;
    }

    const Identifier theNotaryID{server}, theNymID{mynym},
        theAssetType{assetType}, theAcctID{myacct};

    string assetContract = SwigWrap::GetAssetType_Contract(assetType);
    if ("" == assetContract) {
        string response =
            OT::App()
                .API()
                .ServerAction()
                .DownloadContract(theNymID, theNotaryID, theAssetType)
                ->Run();
        if (1 != responseStatus(response)) {
            otOut << "Error: cannot retrieve asset contract.\n";
            return -1;
        }

        assetContract = SwigWrap::GetAssetType_Contract(assetType);
        if ("" == assetContract) {
            otOut << "Error: cannot load asset contract.\n";
            return -1;
        }
    }

    string mint = load_or_retrieve_mint(server, mynym, assetType);
    if ("" == mint) {
        otOut << "Error: cannot load asset mint.\n";
        return -1;
    }

    string response =
        OT::App()
            .API()
            .ServerAction()
            .WithdrawCash(theNymID, theNotaryID, theAcctID, amount)
            ->Run();
    int32_t reply =
        responseReply(response, server, mynym, myacct, "withdraw_cash");
    if (1 != reply) {
        return reply;
    }

    if (!OT::App().API().ServerAction().DownloadAccount(
            theNymID, theNotaryID, theAcctID, true)) {
        otOut << "Error retrieving intermediary files for account.\n";
        return -1;
    }

    return 1;
#else
    return -1;
#endif  // OT_CASH
}
