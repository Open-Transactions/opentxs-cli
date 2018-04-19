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

#include "CmdEditServer.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdEditServer::CmdEditServer()
{
    command = "editserver";
    args[0] = "--server <server>";
    args[1] = "--label <label>";
    category = catWallet;
    help = "Edit server's label, as it appears in your wallet.";
}

CmdEditServer::~CmdEditServer() {}

int32_t CmdEditServer::runWithOptions()
{
    return run(getOption("server"), getOption("label"));
}

int32_t CmdEditServer::run(string server, string label)
{
    if (!checkServer("server", server)) {
        return -1;
    }

    if (!checkMandatory("label", label)) {
        return -1;
    }

    if (!SwigWrap::SetServer_Name(server, label)) {
        otOut << "Error: cannot set server label.\n";
        return -1;
    }

    return 1;
}
