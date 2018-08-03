// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdShowMint.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdShowMint::CmdShowMint()
{
    command = "showmint";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--mypurse <purse>";
    category = catAdmin;
    help = "Show mint file for specific instrument definition id. Download if "
           "necessary.";
}

CmdShowMint::~CmdShowMint() {}

int32_t CmdShowMint::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("mypurse"));
}

std::string CmdShowMint::load_or_retrieve_mint(
    const std::string& notaryID,
    const std::string& nymID,
    const std::string& unitTypeID) const
{
#if OT_CASH
    std::string response = check_nym(notaryID, nymID, nymID);

    if (1 != VerifyMessageSuccess(response)) {
        otOut << "load_or_retrieve_mint: Cannot verify nym for "
                 "IDs: \n";
        otOut << "   Notary ID: " << notaryID << "\n";
        otOut << "      Nym ID: " << nymID << "\n";
        otOut << "Unit Type Id: " << unitTypeID << "\n";
        return "";
    }

    // HERE, WE MAKE SURE WE HAVE THE PROPER MINT...
    //
    // Download the public mintfile if it's not there, or if it's expired.
    // Also load it up into memory as a std::string (just to make sure it
    // works.)

    // expired or missing.
    if (!SwigWrap::Mint_IsStillGood(notaryID, unitTypeID)) {
        otWarn << "load_or_retrieve_mint: Mint file is "
                  "missing or expired. Downloading from "
                  "server...\n";

        std::string response;
        {
            response = OT::App()
                           .Client()
                           .ServerAction()
                           .DownloadMint(
                               Identifier::Factory(nymID),
                               Identifier::Factory(notaryID),
                               Identifier::Factory(unitTypeID))
                           ->Run();
        }

        if (1 != VerifyMessageSuccess(response)) {
            otOut << "load_or_retrieve_mint: Unable to "
                     "retrieve mint for IDs: \n";
            otOut << "   Notary ID: " << notaryID << "\n";
            otOut << "      Nym ID: " << nymID << "\n";
            otOut << "Unit Type Id: " << unitTypeID << "\n";
            return "";
        }

        if (!SwigWrap::Mint_IsStillGood(notaryID, unitTypeID)) {
            otOut << "load_or_retrieve_mint: Retrieved "
                     "mint, but still 'not good' for IDs: \n";
            otOut << "   Notary ID: " << notaryID << "\n";
            otOut << "      Nym ID: " << nymID << "\n";
            otOut << "Unit Type Id: " << unitTypeID << "\n";
            return "";
        }
    }
    // else // current mint IS available already on local storage (and not
    // expired.)

    // By this point, the mint is definitely good, whether we had to download it
    // or not.
    // It's here, and it's NOT expired. (Or we would have returned already.)

    std::string strMint = SwigWrap::LoadMint(notaryID, unitTypeID);
    if (!VerifyStringVal(strMint)) {
        otOut << "load_or_retrieve_mint: Unable to load mint "
                 "for IDs: \n";
        otOut << "   Notary ID: " << notaryID << "\n";
        otOut << "      Nym ID: " << nymID << "\n";
        otOut << "Unit Type Id: " << unitTypeID << "\n";
    }

    return strMint;
#else
    return "";
#endif
}

int32_t CmdShowMint::run(string server, string mynym, string mypurse)
{
#if OT_CASH
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    if (!checkPurse("mypurse", mypurse)) { return -1; }

    string mint = load_or_retrieve_mint(server, mynym, mypurse);
    if ("" == mint) {
        otOut << "Error: cannot load mint.\n";
        return -1;
    }

    cout << mint << "\n";

    return 1;
#else
    return -1;
#endif  // OT_CASH
}
