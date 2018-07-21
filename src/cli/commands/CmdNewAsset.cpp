// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdNewAsset.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdNewAsset::CmdNewAsset()
{
    command = "newasset";
    args[0] = "--mynym <nym>";
    args[1] = "--name <unit name>";
    args[2] = "--shortname <currency description>";
    args[3] = "--symbol <unit symbol>";
    args[4] = "--tla <unit three-leter acronym>";
    args[5] = "--power <decimal power>";
    args[6] = "--fraction <unit fraction name>";
    category = catAdmin;
    help = "Create a new currency contract.";
}

CmdNewAsset::~CmdNewAsset() {}

int32_t CmdNewAsset::runWithOptions()
{
    return run(
        getOption("mynym"),
        getOption("shortname"),
        getOption("name"),
        getOption("symbol"),
        getOption("tla"),
        getOption("power"),
        getOption("fraction"));
}

int32_t CmdNewAsset::run(
    std::string mynym,
    std::string shortname,
    std::string name,
    std::string symbol,
    std::string tla,
    std::string power,
    std::string fraction)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    string input = inputText("an unit definition");
    if ("" == input) { return -1; }

    string unitDefinitionID = SwigWrap::CreateCurrencyContract(
        mynym, shortname, input, name, symbol, tla, stoi(power), fraction);

    if ("" == unitDefinitionID) {
        otOut << "Error: cannot create unit definition.\n";
        return -1;
    }

    cout << "New instrument definition ID : " << unitDefinitionID << std::endl;

    string contract = SwigWrap::GetAssetType_Contract(unitDefinitionID);
    if ("" == contract) {
        otOut << "Error: cannot load unit definition.\n";
        return -1;
    }

    cout << contract << std::endl;

    return 1;
}
