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

#include "CmdShowBasket.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

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
        otOut << "Error: cannot load assetType type list item count.\n";
        return -1;
    }

    if (0 == items) {
        otOut << "The instrument definition list is empty.\n";
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
        otOut << "Error: cannot load instrument definition " << messageNr
              << ".\n";
        return -1;
    }

    string assetName = SwigWrap::GetAssetType_Name(assetType);
    if (!SwigWrap::IsBasketCurrency(assetType)) {
        otOut << "Error: not a basket currency: " << assetType << " : "
              << assetName << ".\n";
        return -1;
    }

    int32_t currencies = SwigWrap::Basket_GetMemberCount(assetType);
    if (0 >= currencies) {
        otOut << "Error: cannot load basket currency count.\n";
        return -1;
    }

    int64_t minAmount = SwigWrap::Basket_GetMinimumTransferAmount(assetType);
    if (0 > minAmount) {
        otOut << "Error: cannot load minimum transfer amount.\n";
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
