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

#include "CmdExportCash.hpp"

#include "CmdBase.hpp"

#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/api/OT.hpp>
#include <opentxs/client/MadeEasy.hpp>
#include <opentxs/client/SwigWrap.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>

using namespace opentxs;
using namespace std;

CmdExportCash::CmdExportCash()
{
    command = "exportcash";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--mypurse <purse>";
    args[3] = "[--hisnym <nym>]";
    args[4] = "[--indices <indices|all>]";
    args[5] = "[--password <true|false>]";
    category = catInstruments;
    help = "Export a cash purse.";
    usage = "When password-protected, --hisnym is ignored.\n"
            "Otherwise cash is exported to hisnym instead of mynym.";
}

CmdExportCash::~CmdExportCash()
{
}

int32_t CmdExportCash::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("mypurse"),
               getOption("hisnym"), getOption("indices"),
               getOption("password"));
}

int32_t CmdExportCash::run(string server, string mynym, string mypurse,
                           string hisnym, string indices, string password)
{
    if (!checkServer("server", server)) {
        return -1;
    }

    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    if (!checkPurse("mypurse", mypurse)) {
        return -1;
    }

    if ("" != password && !checkFlag("password", password)) {
        return -1;
    }

    // if password is true, hisnym becomes optional
    if (password != "true" && "" != hisnym && !checkNym("hisnym", hisnym)) {
        return -1;
    }

    if ("" != indices && !checkIndices("indices", indices)) {
        return -1;
    }

    if (password != "true" && "" == hisnym) {
        hisnym = mynym;
    }

    string retainedCopy = "";
    string purse = exportCash(server, mynym, mypurse, hisnym, indices,
                              password == "true", retainedCopy);
    if ("" == purse) {
        return -1;
    }

    cout << purse << "\n";

    return 1;
}

// MyPurse and HisPurse will call the script even when not found, giving the
// script the opportunity to download the appropriate asset contract from the
// server, if necessary.
//
// Similarly, HisAcct works without being expected to be found in the wallet
// (since maybe it's HIS account and thus it's not IN your wallet...)
//
// Similarly, HisNym will call the script even when not found, giving the script
// the opportunity to download the appropriate pubkey ("check_nym" aka
// "checknym") and continue operating.
//
// All of the above, plus Server, ALREADY attempt a partial match search.
// Therefore, it's not necessary to perform ANOTHER partial match, when the
// value comes from --server, --mynym, --hisnym, --mypurse, --hispurse,
// --myacct, or --hisacct.
//
// You only need to do partial matches when you get values from ELSEWHERE, such
// as from custom arguments, or user-pasted input.
//
// Therefore, add a function for downloading a Nym's pubkey if not already in
// the wallet, and one that downloads an asset contract if not already in the
// wallet.

// The first three arguments are for loading up the purse we're exporting FROM.
//
// Then hisnym is for the Nym we're exporting TO (if hasPassword is false.)
//
// Then indices contains the indices for the tokens to export.
//
// Then hasPassword tells us whether to export to hisnym, or to create a
// password-protected purse and export to that instead. (In which case, the
// purse itself becomes the "owner" and can be passed wherever we would
// normally pass a NymID as a purse owner.)
//
// if hasPassword is false, then hisnym needs to contain the recipient Nym.
// This will contain MyNym by default, if HisNym wasn't provided.

string CmdExportCash::exportCash(const string& server, const string& mynym,
                                 const string& assetType, string& hisnym,
                                 const string& indices, bool hasPassword,
                                 string& retainedCopy) const
{
    string contract =
        OT::App().API().ME().load_or_retrieve_contract(server, mynym, assetType);
    if ("" == contract) {
        otOut << "Error: cannot load asset contract.\n";
        return "";
    }

    string instrument = SwigWrap::LoadPurse(server, assetType, mynym);
    if ("" == instrument) {
        otOut << "Error: cannot load purse.\n";
        return "";
    }

    vector<string> tokens;
    if (0 > getTokens(tokens, server, mynym, assetType, instrument, indices)) {
        return "";
    }

    return OT::App().API().ME().exportCashPurse(server, assetType, mynym, instrument,
                                     tokens, hisnym, hasPassword, retainedCopy);
}
