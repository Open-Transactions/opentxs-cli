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

#include "CmdGetReceipt.hpp"

#include "CmdBase.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/OT_ME.hpp>
#include <opentxs/client/SwigWrap.hpp>
#include <opentxs/core/Log.hpp>

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdGetReceipt::CmdGetReceipt()
{
    command = "getreceipt";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "[--myacct <account>]";
    args[3] = "--id <transactionnr>";
    args[4] = "[--boxtype <0|1|2>]";
    category = catAdmin;
    help = "Downloads a box receipt based on transaction nr.";
    usage = "Box types: 0 = NymBox, 1 = Inbox(default), 2 = Outbox.\n"
            "For Inbox and Outbox you need to specify --myacct";
}

CmdGetReceipt::~CmdGetReceipt()
{
}

int32_t CmdGetReceipt::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("myacct"),
               getOption("id"), getOption("boxtype"));
}

int32_t CmdGetReceipt::run(string server, string mynym, string myacct,
                           string id, string boxtype)
{
    if (!checkServer("server", server)) {
        return -1;
    }

    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    if (!checkTransNum("id", id)) {
        return -1;
    }

    int32_t type = "" == boxtype ? 1 : checkIndex("boxtype", boxtype, 3);
    if (0 > type) {
        return -1;
    }

    if (0 == type) {
        myacct = mynym;
    }
    else {
        if (!checkAccount("myacct", myacct)) {
            return -1;
        }

        server = SwigWrap::GetAccountWallet_NotaryID(myacct);
        if ("" == server) {
            otOut << "Error: cannot determine server from myacct.\n";
            return -1;
        }

        mynym = SwigWrap::GetAccountWallet_NymID(myacct);
        if ("" == mynym) {
            otOut << "Error: cannot determine mynym from myacct.\n";
            return -1;
        }
    }


    int64_t i;
    sscanf(id.c_str(), "%" SCNd64, &i);
    string response = OT_ME::It().get_box_receipt(server, mynym, myacct, type, i);
    return processResponse(response, "get box receipt");
}
