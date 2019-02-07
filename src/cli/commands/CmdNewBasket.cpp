// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdNewBasket.hpp"
#include "CmdShowAssets.hpp"

#include <opentxs/opentxs.hpp>

#include <iostream>
#include <stdint.h>
#include <string>

#define OT_METHOD "opentxs::CmdNewBasket::"

using namespace opentxs;
using namespace std;

CmdNewBasket::CmdNewBasket()
{
    command = "newbasket";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--assets <number of currencies in the basket>";
    args[3] = "--shortname <currency description>";
    args[4] = "--name <unit name>";
    args[5] = "--symbol <unit symbol>";
    args[6] = "--weight <minTransfer>";
    category = catBaskets;
    help = "Create a new basket currency.";
}

CmdNewBasket::~CmdNewBasket() {}

int32_t CmdNewBasket::runWithOptions()
{
    return run(
        getOption("server"),
        getOption("mynym"),
        getOption("assets"),
        getOption("shortname"),
        getOption("name"),
        getOption("symbol"),
        getOption("weight"));
}

int32_t CmdNewBasket::run(
    std::string server,
    std::string mynym,
    std::string assets,
    std::string shortname,
    std::string name,
    std::string symbol,
    std::string weight)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    if (!checkValue("assets", assets)) { return -1; }

    int32_t assetCount = stol(assets);
    if (assetCount < 2) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: invalid asset count for basket.")
            .Flush();
        return -1;
    }

    if (!checkValue("weight", weight)) { return -1; }

    int64_t minTransfer = stoll(weight);

    if (minTransfer < 1) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: invalid minimum transfer amount for basket.")
            .Flush();
        return -1;
    }
    uint64_t intWeight = minTransfer;
    string str_terms = "basket";  // No terms are allowed for basket currencies.

    if ("" == str_terms) { return -1; }

    string basket = SwigWrap::GenerateBasketCreation(
        server, shortname, name, symbol, str_terms, intWeight);

    if ("" == basket) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot create basket.")
            .Flush();
        return -1;
    }

    for (int32_t i = 0; i < assetCount; i++) {
        CmdShowAssets showAssets;
        showAssets.run();

        LogNormal(OT_METHOD)(__FUNCTION__)(": This basket currency has ")(
            assetCount)(" subcurrencies.")
            .Flush();
        LogNormal(OT_METHOD)(__FUNCTION__)(": So far you have defined ")(i)(
            " of them.")
            .Flush();
        LogNormal(OT_METHOD)(__FUNCTION__)(": Please PASTE the instrument "
                                           "definition ID for a subcurrency of "
                                           "this basket: ")
            .Flush();

        string assetType = inputLine();
        if ("" == assetType) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: empty instrument definition.")
                .Flush();
            return -1;
        }

        string assetContract = SwigWrap::GetAssetType_Contract(assetType);
        if ("" == assetContract) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: invalid instrument definition.")
                .Flush();
            i--;
            continue;
        }

        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Enter minimum transfer amount for "
            "that instrument definition "
            "[100]: ")
            .Flush();
        minTransfer = 100;
        string minAmount = inputLine();
        if ("" != minAmount) {
            minTransfer = SwigWrap::StringToAmount(assetType, minAmount);
            if (1 > minTransfer) {
                LogNormal(OT_METHOD)(__FUNCTION__)(
                    ": Error: invalid minimum transfer amount.")
                    .Flush();
                i--;
                continue;
            }
        }

        basket =
            SwigWrap::AddBasketCreationItem(basket, assetType, minTransfer);

        if ("" == basket) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: cannot create basket item.")
                .Flush();
            return -1;
        }
    }

    LogNormal(OT_METHOD)(__FUNCTION__)(": Here's the basket we're issuing: ")(
        basket)
        .Flush();

    std::string response;
    {
        response = Opentxs::Client()
                       .ServerAction()
                       .IssueBasketCurrency(
                           Identifier::Factory(mynym),
                           Identifier::Factory(server),
                           proto::StringToProto<proto::UnitDefinition>(
                               String::Factory(basket.c_str())))
                       ->Run();
    }
    int32_t status = responseStatus(response);
    switch (status) {
        case 1: {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": SUCCESS in issue_basket_currency! "
                "Server response: ")(basket)
                .Flush();
            cout << response << "\n";

            string strNewID =
                SwigWrap::Message_GetNewInstrumentDefinitionID(response);
            bool bGotNewID = "" != strNewID;
            bool bRetrieved = false;

            if (bGotNewID) {
                auto task = Opentxs::Client().OTX().DownloadContract(
                    Identifier::Factory(mynym),
                    Identifier::Factory(server),
                    Identifier::Factory(strNewID));

                const auto result = std::get<1>(task).get();
                const auto success =
                    CmdBase::GetResultSuccess(result);

                if (success) { bRetrieved = true; }
            }
            LogNormal(OT_METHOD)(__FUNCTION__)(
                bRetrieved ? "Success"
                           : "Failed")(" retrieving new basket contract")
                .Flush();
            break;
        }
        case 0: {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": FAILURE in issue_basket_currency! Server "
                "response:")
                .Flush();
            cout << response << "\n";
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": FAILURE in issue_basket_currency!")
                .Flush();
        } break;
        default: {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error in issue_basket_currency! "
                "status is: ")(status)
                .Flush();
        }
            if ("" != response) {
                LogNormal(OT_METHOD)(__FUNCTION__)(": Server response: ")
                    .Flush();
                cout << response << "\n";
                LogNormal(OT_METHOD)(__FUNCTION__)(
                    ": Error in issue_basket_currency! status is: ")(status)
                    .Flush();
            }
            break;
    }
    LogNormal(OT_METHOD)(__FUNCTION__)(" ").Flush();

    return (0 == status) ? -1 : status;
}
