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

#include "CmdExchangeBasket.hpp"

#include "CmdBase.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/OTAPI_Wrap.hpp>
#include <opentxs/client/OT_ME.hpp>
#include <opentxs/client/MadeEasy.hpp>
#include <opentxs/core/util/Common.hpp>
#include <opentxs/core/app/App.hpp>
#include <opentxs/core/app/Api.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdExchangeBasket::CmdExchangeBasket()
{
    command = "exchangebasket";
    args[0] = "--myacct <basketaccount>";
    args[1] = "[--direction <in|out> (default in)]";
    args[2] = "[--multiple <multiple> (default 1)]";
    category = catBaskets;
    help = "Exchange in or out of a basket currency.";
}

CmdExchangeBasket::~CmdExchangeBasket()
{
}

int32_t CmdExchangeBasket::runWithOptions()
{
    return run(getOption("myacct"), getOption("direction"),
               getOption("multiple"));
}

int32_t CmdExchangeBasket::run(string myacct, string direction, string multiple)
{
    if (!checkAccount("myacct", myacct)) {
        return -1;
    }

    if ("" != direction && "in" != direction && "out" != direction) {
        otOut << "Error: direction: expected 'in' or 'out'.\n";
        return -1;
    }

    if ("" != multiple && !checkValue("multiple", multiple)) {
        return -1;
    }

    int32_t multiplier = "" != multiple ? stol(multiple) : 1;
    if (1 > multiplier) {
        otOut << "Error: multiple: invalid value: " << multiple << "\n";
        return -1;
    }

    string server = OTAPI_Wrap::GetAccountWallet_NotaryID(myacct);
    if ("" == server) {
        otOut << "Error: cannot determine server from myacct.\n";
        return -1;
    }

    string mynym = OTAPI_Wrap::GetAccountWallet_NymID(myacct);
    if ("" == mynym) {
        otOut << "Error: cannot determine mynym from myacct.\n";
        return -1;
    }

    string assetType = getAccountAssetType(myacct);
    if ("" == assetType) {
        return -1;
    }

    if (!OTAPI_Wrap::IsBasketCurrency(assetType)) {
        otOut << "Error: account is not a basket currency.\n";
        return -1;
    }

    int32_t members = OTAPI_Wrap::Basket_GetMemberCount(assetType);
    if (2 > members) {
        otOut << "Error: cannot load basket member count.\n";
        return -1;
    }

    int64_t minAmount = OTAPI_Wrap::Basket_GetMinimumTransferAmount(assetType);
    if (0 > minAmount) {
        otOut << "Error: cannot load minimum transfer amount for basket.\n";
        return -1;
    }

    int64_t balance = OTAPI_Wrap::GetAccountWallet_Balance(myacct);
    if (OT_ERROR_AMOUNT == balance) {
        otOut << "Error: cannot retrieve balance for basket account.\n";
        return -1;
    }

    int64_t amount = multiplier * minAmount;

    bool bExchangingIn = "out" != direction;
    if (!bExchangingIn) {
        if (balance < minAmount) {
            otOut << "Error: balance (" << balance
                  << ") is less than minimum amount (" << minAmount << ").\n";
            return -1;
        }

        if (amount > balance) {
            otOut << "Error: balance (" << balance
                  << ") is insufficient for transfer amount (" << amount
                  << ").\n";
            return -1;
        }
    }

     
    if (!OT_ME::It().make_sure_enough_trans_nums(20, server, mynym)) {
        otOut << "Error: cannot reserve transaction numbers.\n";
        return -1;
    }

    string basket = OTAPI_Wrap::GenerateBasketExchange(server, mynym, assetType,
                                                       myacct, multiplier);
    if ("" == basket) {
        otOut << "Error: cannot generate basket exchange.\n";
        return -1;
    }

    // Sub-currencies!
    for (int32_t member = 0; member < members; member++) {
        string memberType = OTAPI_Wrap::Basket_GetMemberType(assetType, member);
        if ("" == memberType) {
            otOut << "Error: cannot load basket member type.\n";
            return harvestTxNumbers(basket, mynym);
        }

        int64_t memberAmount =
            OTAPI_Wrap::Basket_GetMemberMinimumTransferAmount(assetType,
                                                              member);
        if (0 > memberAmount) {
            otOut << "Error: cannot load basket member minimum transfer "
                     "amount.\n";
            return harvestTxNumbers(basket, mynym);
        }

        amount = multiplier * memberAmount;

        if (0 > showBasketAccounts(server, mynym, memberType, true)) {
            return harvestTxNumbers(basket, mynym);
        }

        string memberTypeName = OTAPI_Wrap::GetAssetType_Name(memberType);
        otOut << "\nThere are " << (members - member)
              << " accounts remaining to be selected.\n\n";
        otOut << "Currently we need to select an account with the instrument "
                 "definition:\n" << memberType << " (" << memberTypeName
              << ")\n";
        otOut << "Above are all the accounts in the wallet, for the relevant "
                 "server and nym, of that instrument definition.\n";

        if (bExchangingIn) {
            otOut << "\nKeep in mind, with a transfer multiple of "
                  << multiplier << " and a minimum transfer amount of "
                  << memberAmount
                  << "\n(for this sub-currency), you must therefore select an "
                     "account with a minimum\nbalance of: " << amount << "\n";
        }

        otOut << "\nPlease PASTE an account ID from the above list: ";
        string account = inputLine();
        if ("" == account) {
            otOut << "Error: invalid account ID.\n";
            return harvestTxNumbers(basket, mynym);
        }

        string subAssetType =
            OTAPI_Wrap::GetAccountWallet_InstrumentDefinitionID(account);
        if ("" == subAssetType) {
            otOut << "Error: cannot load account instrument definition.\n";
            return harvestTxNumbers(basket, mynym);
        }

        if (memberType != subAssetType) {
            otOut << "Error: incorrect account instrument definition.\n";
            return harvestTxNumbers(basket, mynym);
        }

        balance = OTAPI_Wrap::GetAccountWallet_Balance(account);
        if (OT_ERROR_AMOUNT == balance) {
            otOut << "Error: cannot load account balance.\n";
            return harvestTxNumbers(basket, mynym);
        }

        if (bExchangingIn && amount > balance) {
            otOut << "Error: account balance (" << balance
                  << ") is insufficient for transfer amount (" << amount
                  << ").\n";
            return harvestTxNumbers(basket, mynym);
        }

        string newBasket = OTAPI_Wrap::AddBasketExchangeItem(
            server, mynym, basket, subAssetType, account);
        if ("" == newBasket) {
            otOut << "Error: cannot add basket exchange item.\n";
            return harvestTxNumbers(basket, mynym);
        }

        basket = newBasket;
    }

    string response = App::Me().API().ME().exchange_basket_currency(
        server, mynym, assetType, basket, myacct, bExchangingIn);
    int32_t reply =
        responseReply(response, server, mynym, myacct, "exchange_basket");
    if (1 != reply) {
        return reply;
    }

    if (!App::Me().API().ME().retrieve_account(server, mynym, myacct, true)) {
        otOut << "Error retrieving intermediary files for account.\n";
        return -1;
    }

    return 1;
}

// Used by exchange_basket for displaying certain types of accounts.
//
// if assetType doesn't exist, it will ONLY show accounts that are basket
// currencies.
// if assetType exists, and bFilter is TRUE, it will ONLY show accounts of
// that type.
// if assetType exists, and bFilter is FALSE, it will only show accounts
// that are NOT of that type.
//
// Also: if server exists, the accounts are filtered by that server.
// Also: if mynym exists, the accounts are filtered by that Nym.
//
int32_t CmdExchangeBasket::showBasketAccounts(const string& server,
                                              const string& mynym,
                                              const string& assetType,
                                              bool bFilter)
{
    int32_t items = OTAPI_Wrap::GetAccountCount();
    if (0 > items) {
        otOut << "Error: cannot load account count.\n";
        return -1;
    }

    dashLine();
    cout << " ** ACCOUNTS:\n\n";

    for (int32_t i = 0; i < items; i++) {
        string acct = OTAPI_Wrap::GetAccountWallet_ID(i);
        if ("" == acct) {
            otOut << "Error: cannot load account ID.\n";
            return -1;
        }

        string accountServer = OTAPI_Wrap::GetAccountWallet_NotaryID(acct);
        if ("" == accountServer) {
            otOut << "Error: cannot determine server from myacct.\n";
            return -1;
        }

        if ("" == server || server == accountServer) {
            string accountNym = OTAPI_Wrap::GetAccountWallet_NymID(acct);
            if ("" == accountNym) {
                otOut << "Error: cannot determine accountNym from acct.\n";
                return -1;
            }

            if ("" == mynym || mynym == accountNym) {
                string asset = getAccountAssetType(acct);
                if ("" == asset) {
                    return -1;
                }

                if (("" == assetType && OTAPI_Wrap::IsBasketCurrency(asset)) ||
                    ("" != assetType && bFilter && assetType == asset) ||
                    ("" != assetType && !bFilter && assetType != asset)) {
                    string statAccount = App::Me().API().ME().stat_asset_account(acct);
                    if ("" == statAccount) {
                        otOut << "Error: cannot stat account.\n";
                        return -1;
                    }

                    if (0 < i) {
                        cout << "-------------------------------------\n";
                    }
                    cout << statAccount << "\n";
                }
            }
        }
    }
    dashLine();

    return 1;
}
