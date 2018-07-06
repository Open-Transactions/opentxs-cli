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

#include "CmdAcceptMoney.hpp"

#include <opentxs/opentxs.hpp>

#include <cstdint>
#include <string>

using namespace opentxs;
using namespace std;

CmdAcceptMoney::CmdAcceptMoney()
{
    command = "acceptmoney";
    args[0] = "--myacct <account>";
    args[1] = "[--server <server>]";
    category = catAccounts;
    help = "Accept all incoming transfers and payments into myacct.";
    usage = "Server is notary where I receive messages (and payments). "
            "FYI, the default server is myacct's NotaryId.";
}

CmdAcceptMoney::~CmdAcceptMoney() {}

std::int32_t CmdAcceptMoney::runWithOptions()
{
    return run(getOption("server"), getOption("myacct"));
}

std::int32_t CmdAcceptMoney::run(string server, string myacct)
{
    if (!checkAccount("myacct", myacct)) { return -1; }

    if (!checkServer("server", server)) {
        server = SwigWrap::GetAccountWallet_NotaryID(myacct);
    }
    if (!checkServer("server", server)) { return -1; }
    string& transport_notary = server;

    // FIX: these OR's should become AND's so we can detect any failure
    bool success =
        0 <= acceptFromInbox(myacct, "all", OTRecordList::typeTransfers);
    success |=
        0 <= acceptFromPaymentbox(transport_notary, myacct, "all", "PURSE");
    success |=
        0 <= acceptFromPaymentbox(transport_notary, myacct, "all", "CHEQUE");
    return success ? 1 : -1;
}
