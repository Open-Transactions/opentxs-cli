// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdDiscard.hpp"

#include <opentxs/opentxs.hpp>

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

CmdDiscard::~CmdDiscard() {}

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
    return opentxs::cli::RecordList::discard_incoming_payments(server, mynym, indices);
}
