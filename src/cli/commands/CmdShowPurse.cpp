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

#include "CmdShowPurse.hpp"

#include "CmdBase.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/OTAPI_Wrap.hpp>
#include <opentxs/core/Log.hpp>
#include <opentxs/core/util/Common.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdShowPurse::CmdShowPurse()
{
    command = "showpurse";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--mypurse <purse>";
    category = catWallet;
    help = "Show contents of a cash purse.";
}

CmdShowPurse::~CmdShowPurse()
{
}

int32_t CmdShowPurse::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("mypurse"));
}

int32_t CmdShowPurse::run(string server, string mynym, string mypurse)
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

    string purse = OTAPI_Wrap::LoadPurse(server, mypurse, mynym);
    if ("" == purse) {
        otOut << "Error: cannt load purse.\n";
        return -1;
    }

    int64_t amount = OTAPI_Wrap::Purse_GetTotalValue(server, mypurse, purse);
    cout << "Total value: " << OTAPI_Wrap::FormatAmount(mypurse, amount)
         << "\n";

    int32_t items = OTAPI_Wrap::Purse_Count(server, mypurse, purse);
    if (0 > items) {
        otOut << "Error: cannot load purse item count.\n";
        return -1;
    }

    if (0 == items) {
        otOut << "The purse is empty.\n";
        return 0;
    }

    time64_t now = OTAPI_Wrap::GetTime();
    if (OT_TIME_ZERO > now) {
        otOut << "Error: cannot get current time.\n";
        return -1;
    }

    cout << "Token count: " << items << "\n";
    cout << "Index\tValue\tSeries\tValidFrom\tValidTo\t\tStatus\n";

    for (int32_t i = 0; i < items; i++) {
        string token = OTAPI_Wrap::Purse_Peek(server, mypurse, mynym, purse);
        if ("" == token) {
            otOut << "Error: cannot load token " << i << ".\n";
            return -1;
        }

        purse = OTAPI_Wrap::Purse_Pop(server, mypurse, mynym, purse);
        if ("" == purse) {
            otOut << "Error: cannot load updated purse.\n";
            return -1;
        }

        int64_t denomination =
            OTAPI_Wrap::Token_GetDenomination(server, mypurse, token);
        if (0 > denomination) {
            otOut << "Error: cannot load denomination.\n";
            return -1;
        }

        int32_t series = OTAPI_Wrap::Token_GetSeries(server, mypurse, token);
        if (0 > series) {
            otOut << "Error: cannot load series.\n";
            return -1;
        }

        time64_t from = OTAPI_Wrap::Token_GetValidFrom(server, mypurse, token);
        if (OT_TIME_ZERO > from) {
            otOut << "Error: cannot load validFrom.\n";
            return -1;
        }

        time64_t until = OTAPI_Wrap::Token_GetValidTo(server, mypurse, token);
        if (OT_TIME_ZERO > until) {
            otOut << "Error: cannot load validTo.\n";
            return -1;
        }

        string status = until < now ? "expired" : "valid";

        cout << i << "\t" << denomination << "\t" << series << "\t" << from
             << "\t" << until << "\t" << status << "\n";
    }

    return 1;
}
