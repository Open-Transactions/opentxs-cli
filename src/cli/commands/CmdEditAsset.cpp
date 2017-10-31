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

#include "CmdEditAsset.hpp"

#include "CmdBase.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/SwigWrap.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdEditAsset::CmdEditAsset()
{
    command = "editasset";
    args[0] = "--mypurse <purse>";
    args[1] = "--label <label>";
    category = catWallet;
    help = "Edit mypurse's label, as it appears in your wallet.";
}

CmdEditAsset::~CmdEditAsset()
{
}

int32_t CmdEditAsset::runWithOptions()
{
    return run(getOption("mypurse"), getOption("label"));
}

int32_t CmdEditAsset::run(string mypurse, string label)
{
    if (!checkPurse("mypurse", mypurse)) {
        return -1;
    }

    if (!checkMandatory("label", label)) {
        return -1;
    }

    if (!SwigWrap::SetAssetType_Name(mypurse, label)) {
        otOut << "Error: cannot set purse label.\n";
        return -1;
    }

    return 1;
}
