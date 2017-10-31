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

#include "CmdDiscard.hpp"

#include "CmdBase.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/SwigWrap.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdDiscard::CmdDiscard()
{
    command = "discard";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--indices <indices|all>";
    category = catInstruments;
    help = "Discard uncashed incoming instruments from payments inbox.";
}

CmdDiscard::~CmdDiscard()
{
}

int32_t CmdDiscard::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("indices"));
}

// TODO: Modified cancelCronItem so you can pass an instrument. So even for
// non-cron items, you should still be able to cancel them. (aka cancel the
// transaction numbers on it.) This is for when you change your mind on an
// outgoing instrument, and you want it cancelled before the recipient can
// process it.
// Outgoing cheque, outgoing payment plan, outgoing smart contract, etc.

// AND WHAT ABOUT incoming cheques? I can already discard it, but that leaves
// the sender with it still sitting in his outpayment box, until it expires.
// I'd rather notify him. He should get a failure/rejection notice so that his
// client can also harvest whatever numbers it needs to harvest.

// TODO: CmdCancel and CmdDiscard both need to be updated so that they involve
// a server message. This way the other parties can be notified of the
// cancellation / discarding.

int32_t CmdDiscard::run(string server, string mynym, string indices)
{
    if (!checkServer("server", server)) {
        return -1;
    }

    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    if (!checkIndices("indices", indices)) {
        return -1;
    }

    string inbox = SwigWrap::LoadPaymentInbox(server, mynym);
    if ("" == inbox) {
        otOut << "Error: cannot load payment inbox.\n";
        return -1;
    }

    int32_t items = SwigWrap::Ledger_GetCount(server, mynym, mynym, inbox);
    if (0 > items) {
        otOut << "Error: cannot load payment inbox item count.\n";
        return -1;
    }

    if (0 == items) {
        otOut << "The payment inbox is empty.\n";
        return 0;
    }

    bool all = "all" == indices;

    // Loop from back to front, in case any are removed.
    int32_t retVal = 1;
    for (int32_t i = items - 1; 0 <= i; i--) {
        if (!all && !SwigWrap::NumList_VerifyQuery(indices, to_string(i))) {
            continue;
        }

        if (!SwigWrap::RecordPayment(server, mynym, true, i, false)) {
            otOut << "Error: cannot discard payment.\n";
            retVal = -1;
            continue;
        }

        otOut << "Success discarding payment!\n";
    }

    return retVal;
}
