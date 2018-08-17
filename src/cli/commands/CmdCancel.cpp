// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdCancel.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdCancel::CmdCancel()
{
    command = "cancel";
    args[0] = "--mynym <nym>";
    args[1] = "[--myacct <account>]";
    args[2] = "--indices <indices|all>";
    category = catInstruments;
    help = "Cancel an uncashed outgoing instrument from outpayment box.";
    usage = "Specify --myacct when canceling a smart contract.";
}

CmdCancel::~CmdCancel() {}

int32_t CmdCancel::runWithOptions()
{
    return run(getOption("mynym"), getOption("myacct"), getOption("indices"));
}

// Also TODO:
// Update the "cancel" command, for outgoing cash, to give you a choice to
// deposit the cash instead
// of discarding it.
// CmdDeposit::depositPurse(strNotaryID, strMyAcctID, strToNymID, strInstrument,
// "") // strIndices is left blank in this case

// NOTE: You can't just discard a sent cheque  from your outpayment box.
// Why not? Just because you remove your record of the outgoing cheque,
// doesn't mean you didn't already send it. (The recipient still received
// it, and still has it, whether you remove it from your outbox or not.)
// If you really want to cancel the cheque, then you need to do it in such
// a way that it will fail-as-cancelled when the recipient tries to deposit
// it. Otherwise, he would get that money whether you kept your own outgoing
// record or not. Therefore SOME server message must be performed here,
// which actually cancels the transaction number itself that appears on the
// cheque. This is the only way to insure that the cheque can't be used by
// the recipient (and even this will only work if you beat him to the punch.
// If he deposits it before you cancel it, then it's already too late and he
// has the money.) THIS is why RecordPayment, regarding outpayments, only
// works on expired instruments -- because if it's not expired, you don't
// just want to remove your record of it. You want to cancel the transaction
// number itself -- and that requires server communication.

int32_t CmdCancel::run(string mynym, string myacct, string indices)
{
    return opentxs::cli::RecordList::cancel_outgoing_payments(
        mynym, myacct, indices);
}
