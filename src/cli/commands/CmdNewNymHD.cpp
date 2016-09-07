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

#include "CmdNewNymHD.hpp"

#include "CmdBase.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/OTAPI_Wrap.hpp>
#include <opentxs/client/OT_ME.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdNewNymHD::CmdNewNymHD()
{
    command = "newnymhd";
    args[0] = "--label <label>";
    args[2] = "[--source <seed fingerprint>]";
    category = catNyms;
    help = "create a new nym using HD key derivation.";
}

CmdNewNymHD::~CmdNewNymHD()
{
}

int32_t CmdNewNymHD::runWithOptions()
{
    return run(getOption("label"), getOption("source"));
}

int32_t CmdNewNymHD::run(string label, string source)
{
    if (!checkMandatory("label", label)) {
        return -1;
    }

    OT_ME ot_me;
    string mynym = ot_me.create_nym_hd(source);
    if ("" == mynym) {
        otOut << "Error: cannot create new nym.\n";
        return -1;
    }

    cout << "New nym: " << mynym << "\n";

    if (!OTAPI_Wrap::SetNym_Name(mynym, mynym, label)) {
        otOut << "Error: cannot set new nym name.\n";
        return -1;
    }
    return 1;
}
