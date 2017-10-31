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

#include "CmdIssueAsset.hpp"

#include "CmdBase.hpp"
#include "CmdRegisterNym.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/api/Api.hpp>
#include <opentxs/api/OT.hpp>
#include <opentxs/client/MadeEasy.hpp>
#include <opentxs/client/SwigWrap.hpp>

#include <stdint.h>
#include <string>

using namespace opentxs;
using namespace std;

CmdIssueAsset::CmdIssueAsset()
{
    command = "issueasset";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    category = catAdmin;
    help = "Issue a currency contract onto an OT server.";
    usage = "Mynym must already be the contract key on the new contract.";
}

CmdIssueAsset::~CmdIssueAsset()
{
}

int32_t CmdIssueAsset::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"));
}

int32_t CmdIssueAsset::run(string server, string mynym)
{
    if (!checkServer("server", server)) {
        return -1;
    }

    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    string contract = inputText("an asset contract");
    if ("" == contract) {
        return -1;
    }

    if (!SwigWrap::IsNym_RegisteredAtServer(mynym, server)) {
        CmdRegisterNym registerNym;
        registerNym.run(server, mynym);
    }

    string response = OT::App().API().ME().issue_asset_type(server, mynym, contract);
    return processResponse(response, "issue asset contract");
}
