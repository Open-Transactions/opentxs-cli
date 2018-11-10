// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdShowBasket.hpp"

#include <opentxs/opentxs.hpp>

#include <iostream>
#include <stdint.h>
#include <string>

#define OT_METHOD "opentxs::CmdShowBasket::"

using namespace opentxs;
using namespace std;

CmdShowBasket::CmdShowBasket()
{
    command = "showbasket";
    args[0] = "[--index <index>]";
    category = catBaskets;
    help = "Show basket currency details.";
    usage = "Omitting --indices lists all basket currencies.";
}

CmdShowBasket::~CmdShowBasket() {}

int32_t CmdShowBasket::runWithOptions() { return run(getOption("indices")); }

int32_t CmdShowBasket::run(string index)
{
    int32_t items = SwigWrap::GetAssetTypeCount();
    if (0 > items) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot load assetType type list item count.")
            .Flush();
        return -1;
    }

    if (0 == items) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": The instrument definition list is empty.")
            .Flush();
        return 0;
    }

    // all items?
    if ("" == index) {
        cout << "Index |  Basket currencies:\n";
        dashLine();

        for (int32_t i = 0; i < items; i++) {
            string assetType = SwigWrap::GetAssetType_ID(i);
            if ("" != assetType && SwigWrap::IsBasketCurrency(assetType)) {
                cout << i << ": " << assetType;
                string assetName = SwigWrap::GetAssetType_Name(assetType);
                if ("" != assetName) { cout << " : " + assetName; }
                cout << "\n";
            }
        }

        return 1;
    }

    int32_t messageNr = checkIndex("index", index, items);
    if (0 > messageNr) { return -1; }

    string assetType = SwigWrap::GetAssetType_ID(messageNr);
    if ("" == assetType) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot load instrument definition ")(messageNr)(".")
            .Flush();
        return -1;
    }

    string assetName = SwigWrap::GetAssetType_Name(assetType);
    if (!SwigWrap::IsBasketCurrency(assetType)) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: not a basket currency: ")(
            assetType)(" : ")(assetName)(".")
            .Flush();
        return -1;
    }

    int32_t currencies = SwigWrap::Basket_GetMemberCount(assetType);
    if (0 >= currencies) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot load basket currency count.")
            .Flush();
        return -1;
    }

    int64_t minAmount = SwigWrap::Basket_GetMinimumTransferAmount(assetType);
    if (0 > minAmount) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot load minimum transfer amount.")
            .Flush();
        return -1;
    }

    cout << "Name: " << assetName << "\n";
    cout << "ID:   " << assetType << "\n";

    cout << "Minimum transfer amount for basket:     " << minAmount << "\n";
    cout << "Number of sub-currencies in the basket: " << currencies << "\n";
    cout << "    Index  :  Min Transfer Amount  :  Member currency  \n";
    dashLine();

    for (int32_t i = 0; i < currencies; i++) {
        string type = SwigWrap::Basket_GetMemberType(assetType, i);
        string name = "" != type ? SwigWrap::GetAssetType_Name(type) : "";
        int64_t min =
            SwigWrap::Basket_GetMemberMinimumTransferAmount(assetType, i);

        cout << "    " << i << "      : " << min << " : " << type << " : "
             << name << "\n";
    }

    return 1;
}
