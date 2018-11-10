// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdExchangeBasket.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

#define OT_METHOD "opentxs::CmdExchangeBasket::"

CmdExchangeBasket::CmdExchangeBasket()
{
    command = "exchangebasket";
    args[0] = "--myacct <basketaccount>";
    args[1] = "[--direction <in|out> (default in)]";
    args[2] = "[--multiple <multiple> (default 1)]";
    category = catBaskets;
    help = "Exchange in or out of a basket currency.";
}

CmdExchangeBasket::~CmdExchangeBasket() {}

int32_t CmdExchangeBasket::runWithOptions()
{
    return run(
        getOption("myacct"), getOption("direction"), getOption("multiple"));
}

int32_t CmdExchangeBasket::run(string myacct, string direction, string multiple)
{
    if (!checkAccount("myacct", myacct)) { return -1; }

    if ("" != direction && "in" != direction && "out" != direction) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: direction: expected 'in' or 'out'.")
            .Flush();
        return -1;
    }

    if ("" != multiple && !checkValue("multiple", multiple)) { return -1; }

    int32_t multiplier = "" != multiple ? stol(multiple) : 1;
    if (1 > multiplier) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
	   ": Error: multiple: invalid value: ")(multiple)(".")
            .Flush();
        return -1;
    }

    string server = SwigWrap::GetAccountWallet_NotaryID(myacct);
    if ("" == server) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot determine server from myacct.")
            .Flush();
        return -1;
    }

    string mynym = SwigWrap::GetAccountWallet_NymID(myacct);
    if ("" == mynym) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot determine mynym from myacct.")
            .Flush();
        return -1;
    }

    string assetType = getAccountAssetType(myacct);
    if ("" == assetType) { return -1; }

    if (!SwigWrap::IsBasketCurrency(assetType)) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: account is not a basket currency.")
            .Flush();
        return -1;
    }

    int32_t members = SwigWrap::Basket_GetMemberCount(assetType);
    if (2 > members) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot load basket member count.")
            .Flush();
        return -1;
    }

    int64_t minAmount = SwigWrap::Basket_GetMinimumTransferAmount(assetType);
    if (0 > minAmount) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot load minimum transfer amount for basket.")
            .Flush();
        return -1;
    }

    int64_t balance = SwigWrap::GetAccountWallet_Balance(myacct);
    if (OT_ERROR_AMOUNT == balance) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot retrieve balance for basket account.")
            .Flush();
        return -1;
    }

    int64_t amount = multiplier * minAmount;

    bool bExchangingIn = "out" != direction;
    if (!bExchangingIn) {
        if (balance < minAmount) {
            LogNormal(OT_METHOD)(__FUNCTION__)(": Error: balance (")(balance)(
                ") is less than minimum amount (")(minAmount)(").")
                .Flush();
            return -1;
        }

        if (amount > balance) {
            LogNormal(OT_METHOD)(__FUNCTION__)(": Error: balance (")(balance)(
                ") is insufficient for transfer amount (")(amount)(").")
                .Flush();
            return -1;
        }
    }

    {
        if (!Opentxs::Client().ServerAction().GetTransactionNumbers(
                Identifier::Factory(mynym), Identifier::Factory(server), 20)) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: cannot reserve transaction numbers.")
                .Flush();
            return -1;
        }
    }

    string basket = SwigWrap::GenerateBasketExchange(
        server, mynym, assetType, myacct, multiplier);
    if ("" == basket) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot generate basket exchange.")
            .Flush();
        return -1;
    }

    // Sub-currencies!
    for (int32_t member = 0; member < members; member++) {
        string memberType = SwigWrap::Basket_GetMemberType(assetType, member);
        if ("" == memberType) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: cannot load basket member type.")
                .Flush();
            return harvestTxNumbers(basket, mynym);
        }

        int64_t memberAmount =
            SwigWrap::Basket_GetMemberMinimumTransferAmount(assetType, member);
        if (0 > memberAmount) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: cannot load basket member minimum transfer "
                "amount.")
                .Flush();
            return harvestTxNumbers(basket, mynym);
        }

        amount = multiplier * memberAmount;

        if (0 > showBasketAccounts(server, mynym, memberType, true)) {
            return harvestTxNumbers(basket, mynym);
        }

        string memberTypeName = SwigWrap::GetAssetType_Name(memberType);
        LogNormal(OT_METHOD)(__FUNCTION__)(": There are ")(members - member)(
            " accounts remaining to be selected.")
            .Flush();
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Currently we need to select an account with the instrument "
            "definition:")
            .Flush();
        LogNormal(OT_METHOD)(__FUNCTION__)(": ")(memberType)(" (")(memberTypeName)(
            ").")
            .Flush();
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Above are all the accounts in the wallet, for the relevant "
            "server and nym, of that instrument definition.")
            .Flush();

        if (bExchangingIn) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Keep in mind, with a transfer multiple of ")(multiplier)(
                " and a minimum transfer amount of ")(memberAmount)(".")
                .Flush();
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": (for this sub-currency), you must therefore select an "
                "account with a minimum\nbalance of: ")(amount)(".")
                .Flush();
        }

        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Please PASTE an account ID from the above list: ")
            .Flush();
        string account = inputLine();
        if ("" == account) {
            LogNormal(OT_METHOD)(__FUNCTION__)(": Error: invalid account ID.")
                .Flush();
            return harvestTxNumbers(basket, mynym);
        }

        string subAssetType =
            SwigWrap::GetAccountWallet_InstrumentDefinitionID(account);
        if ("" == subAssetType) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: cannot load account instrument definition.")
                .Flush();
            return harvestTxNumbers(basket, mynym);
        }

        if (memberType != subAssetType) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: incorrect account instrument definition.")
                .Flush();
            return harvestTxNumbers(basket, mynym);
        }

        balance = SwigWrap::GetAccountWallet_Balance(account);
        if (OT_ERROR_AMOUNT == balance) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: cannot load account balance.")
                .Flush();
            return harvestTxNumbers(basket, mynym);
        }

        if (bExchangingIn && amount > balance) {
            LogNormal(OT_METHOD)(__FUNCTION__)(": Error: account balance (")(
                balance)(") is insufficient for transfer amount (")(amount)(
                ").")
                .Flush();
            return harvestTxNumbers(basket, mynym);
        }

        string newBasket = SwigWrap::AddBasketExchangeItem(
            server, mynym, basket, subAssetType, account);
        if ("" == newBasket) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: cannot add basket exchange item.")
                .Flush();
            return harvestTxNumbers(basket, mynym);
        }

        basket = newBasket;
    }

    const OTIdentifier theNotaryID = Identifier::Factory({server}),
                       theNymID = Identifier::Factory({mynym}),
                       theAcctID = Identifier::Factory({myacct});
    std::string response;
    {
        response = Opentxs::Client()
                       .ServerAction()
                       .ExchangeBasketCurrency(
                           theNymID,
                           theNotaryID,
                           Identifier::Factory(assetType),
                           theAcctID,
                           Identifier::Factory(basket),
                           bExchangingIn)
                       ->Run();
    }
    int32_t reply =
        responseReply(response, server, mynym, myacct, "exchange_basket");
    if (1 != reply) { return reply; }

    {
        if (!Opentxs::Client().ServerAction().DownloadAccount(
                theNymID, theNotaryID, theAcctID, true)) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error retrieving intermediary files for account.")
                .Flush();
            return -1;
        }
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
int32_t CmdExchangeBasket::showBasketAccounts(
    const string& server,
    const string& mynym,
    const string& assetType,
    bool bFilter)
{
    const auto& storage = Opentxs::Client().Storage();
    dashLine();
    cout << " ** ACCOUNTS:\n\n";

    for (const auto& it : storage.AccountList()) {
        const auto& acct = std::get<0>(it);

        string accountServer = SwigWrap::GetAccountWallet_NotaryID(acct);
        if ("" == accountServer) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: cannot determine server from myacct.")
                .Flush();
            return -1;
        }

        if ("" == server || server == accountServer) {
            string accountNym = SwigWrap::GetAccountWallet_NymID(acct);
            if ("" == accountNym) {
                LogNormal(OT_METHOD)(__FUNCTION__)(
                    ": Error: cannot determine accountNym from acct.")
                    .Flush();
                return -1;
            }

            if ("" == mynym || mynym == accountNym) {
                string asset = getAccountAssetType(acct);
                if ("" == asset) { return -1; }

                if (("" == assetType && SwigWrap::IsBasketCurrency(asset)) ||
                    ("" != assetType && bFilter && assetType == asset) ||
                    ("" != assetType && !bFilter && assetType != asset)) {
                    string statAccount = stat_asset_account(acct);
                    if ("" == statAccount) {
                        LogNormal(OT_METHOD)(__FUNCTION__)(
                            ": Error: cannot stat account.")
                            .Flush();
                        return -1;
                    }

                    cout << statAccount << "\n";
                }
            }
        }
    }
    dashLine();

    return 1;
}
