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

#include "CmdDeleteInmail.hpp"

#include "CmdBase.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/OTAPI_Wrap.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdDeleteInmail::CmdDeleteInmail()
{
    command = "deleteinmail";
    args[0] = "--mynym <nym>";
    args[1] = "--indices <indices|all>";
    category = catOtherUsers;
    help = "Delete in-mail item(s).";
}

CmdDeleteInmail::~CmdDeleteInmail()
{
}

int32_t CmdDeleteInmail::runWithOptions()
{
    return run(getOption("mynym"), getOption("indices"));
}

int32_t CmdDeleteInmail::run(string mynym, string indices)
{
    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    if (!checkIndices("indices", indices)) {
        return -1;
    }

    int32_t items = OTAPI_Wrap::GetNym_MailCount(mynym);
    if (0 > items) {
        otOut << "Error: cannot retrieve mail inbox item count.\n";
        return -1;
    }

    if (!checkIndicesRange("indices", indices, items)) {
        return -1;
    }

    if (0 == items) {
        otOut << "The mail inbox is empty.\n";
        return 0;
    }

    bool all = "all" == indices;

    int32_t retVal = 1;
    for (int32_t i = items - 1; 0 <= i; i--) {
        // do we want this index?
        if (!all && !OTAPI_Wrap::NumList_VerifyQuery(indices, to_string(i))) {
            continue;
        }

        if (!OTAPI_Wrap::Nym_RemoveMailByIndex(mynym, i)) {
            otOut << "Error: cannot delete inmail at index " << i
                  << " for nym: " << mynym << " \n";
            retVal = -1;
            continue;
        }

        otOut << "Deleted inmail at index " << i << " for nym: " << mynym
              << " \n";
    }

    return retVal;
}
