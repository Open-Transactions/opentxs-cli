// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdNewBasket.hpp"
#include "CmdShowAssets.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

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
        otOut << "Error: invalid asset count for basket.\n";
        return -1;
    }

    if (!checkValue("weight", weight)) { return -1; }

    int64_t minTransfer = stoll(weight);

    if (minTransfer < 1) {
        otOut << "Error: invalid minimum transfer amount for basket.\n";
        return -1;
    }
    uint64_t intWeight = minTransfer;
    string str_terms = "basket";  // No terms are allowed for basket currencies.

    if ("" == str_terms) { return -1; }

    string basket = SwigWrap::GenerateBasketCreation(
        server, shortname, name, symbol, str_terms, intWeight);

    if ("" == basket) {
        otOut << "Error: cannot create basket.\n";
        return -1;
    }

    for (int32_t i = 0; i < assetCount; i++) {
        CmdShowAssets showAssets;
        showAssets.run();

        otOut << std::endl
              << "This basket currency has " << assetCount << " subcurrencies."
              << std::endl;
        otOut << "So far you have defined " << i << " of them." << std::endl;
        otOut << "Please PASTE the instrument definition ID for a subcurrency "
                 "of this "
                 "basket: "
              << std::endl;

        string assetType = inputLine();
        if ("" == assetType) {
            otOut << "Error: empty instrument definition." << std::endl;
            return -1;
        }

        string assetContract = SwigWrap::GetAssetType_Contract(assetType);
        if ("" == assetContract) {
            otOut << "Error: invalid instrument definition." << std::endl;
            i--;
            continue;
        }

        otOut << "Enter minimum transfer amount for that instrument definition "
                 "[100]: "
              << std::endl;
        minTransfer = 100;
        string minAmount = inputLine();
        if ("" != minAmount) {
            minTransfer = SwigWrap::StringToAmount(assetType, minAmount);
            if (1 > minTransfer) {
                otOut << "Error: invalid minimum transfer amount." << std::endl;
                i--;
                continue;
            }
        }

        basket =
            SwigWrap::AddBasketCreationItem(basket, assetType, minTransfer);

        if ("" == basket) {
            otOut << "Error: cannot create basket item.\n";
            return -1;
        }
    }

    otOut << "Here's the basket we're issuing:\n\n" << basket << std::endl;

    std::string response;
    {
        response = Opentxs::
                       Client()
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
            otOut << "\n\n SUCCESS in issue_basket_currency! Server "
                     "response:\n\n";
            cout << response << "\n";

            string strNewID =
                SwigWrap::Message_GetNewInstrumentDefinitionID(response);
            bool bGotNewID = "" != strNewID;
            bool bRetrieved = false;
            string strEnding = ".";

            if (bGotNewID) {
                {
                    response = Opentxs::
                                   Client()
                                   .ServerAction()
                                   .DownloadContract(
                                       Identifier::Factory(mynym),
                                       Identifier::Factory(server),
                                       Identifier::Factory(strNewID))
                                   ->Run();
                }
                strEnding = ": " + strNewID;

                if (1 == responseStatus(response)) { bRetrieved = true; }
            }
            otOut << "Server response: SUCCESS in issue_basket_currency!\n";
            otOut << (bRetrieved ? "Success" : "Failed")
                  << " retrieving new basket contract" << strEnding << "\n";
            break;
        }
        case 0:
            otOut << "\n\n FAILURE in issue_basket_currency! Server "
                     "response:\n\n";
            cout << response << "\n";
            otOut << " FAILURE in issue_basket_currency!\n";
            break;
        default:
            otOut << "\n\nError in issue_basket_currency! status is: " << status
                  << "\n";

            if ("" != response) {
                otOut << "Server response:\n\n";
                cout << response << "\n";
                otOut << "\nError in issue_basket_currency! status is: "
                      << status << "\n";
            }
            break;
    }
    otOut << "\n";

    return (0 == status) ? -1 : status;
}
