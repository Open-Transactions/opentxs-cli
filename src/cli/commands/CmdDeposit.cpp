// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdDeposit.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <ostream>
#include <string>
#include <vector>

using namespace opentxs;
using namespace std;

CmdDeposit::CmdDeposit()
{
    command = "deposit";
    args[0] = "--myacct <account>";
    args[1] = "[--mynym <nym>]";
    args[2] = "[--indices <indices|all>]";
    category = catAccounts;
    help = "Deposit cash, cheque, voucher, or invoice.";
    usage =
        "Any supplied indices must correspond to tokens in your cash purse.";
}

CmdDeposit::~CmdDeposit() {}

int32_t CmdDeposit::runWithOptions()
{
    return run(getOption("mynym"), getOption("myacct"), getOption("indices"));
}

// A bit complicated:
//
// If I specify MyPurse and MyAcct, then they MUST have the same instrument
// definition.
// If I specify MyNym and MyPurse, that is where we will look for the purse.
// If I specify MyAcct, and it's owned by a different Nym than MyNym, then
// the cash tokens will be reassigned from MyNym to MyAcct's Nym, before
// depositing.
// Basically ALWAYS look up MyAcct's owner, and set HIM as the recipient Nym.
// (But still use MyNym, independently, to find the purse being deposited.)
//
// Must ALWAYS specify MyAcct because otherwise, where are you depositing to?
//
// If MyNym isn't available, should use MyAcct's Nym.
//
// Shouldn't need to specify MyPurse, since we can ONLY deposit into MyAcct of
// the same type as MyAcct. Thus we should ignore any other instrument
// definitions or
// purses since they couldn't possibly be deposited into MyAcct anyway.

int32_t CmdDeposit::run(string mynym, string myacct, string indices)
{
    if (!checkAccount("myacct", myacct)) { return -1; }

    string server = SwigWrap::GetAccountWallet_NotaryID(myacct);
    if ("" == server) {
        otOut << "Error: cannot determine server from myacct.\n";
        return -1;
    }

    string toNym = SwigWrap::GetAccountWallet_NymID(myacct);
    if ("" == toNym) {
        otOut << "Error: cannot determine toNym from myacct.\n";
        return -1;
    }

    if ("" != indices) {
        // Only in the case of cash, it's possible you have some cash in Nym A's
        // purse, but you want to deposit it into Nym B's account. So we have a
        // "to" Nym and a "from" Nym even though they will often be the same.
        string fromNym = toNym;
        if (!mynym.empty()) {
            if (!checkNym("mynym", mynym)) { return -1; }
            fromNym = mynym;
        }

        // In this case, instrument is blank. That's how the callee knows that
        // we're working with the local purse. Then indices tells him either to
        // use "all" tokens in that purse, or the selected indices.
        return depositPurse(server, myacct, fromNym, "", indices);
    }

    otOut << "You can deposit a PURSE (containing cash tokens) or "
             "a CHEQUE / INVOICE / VOUCHER.\n";

    string instrument = inputText("your financial instrument");
    if ("" == instrument) { return -1; }

    string type = SwigWrap::Instrmnt_GetType(instrument);
    if ("PURSE" == type) {
        return depositPurse(server, myacct, toNym, instrument, "");
    }

    if ("CHEQUE" == type || "INVOICE" == type || "VOUCHER" == type) {
        return depositCheque(server, myacct, toNym, instrument);
    }

    otOut << "Error: cannot determine instrument type.\n"
             "Expected CHEQUE, VOUCHER, INVOICE, or (cash) PURSE.\n";
    return -1;
}

// THESE FUNCTIONS were added for the PAYMENTS screen. (They are fairly new.)
//
// Basically there was a need to have DIFFERENT instruments, but to be able to
// treat them as though they are a single type.
//
// In keeping with that, the below functions will work with disparate types.
// You can pass [ CHEQUES / VOUCHERS / INVOICES ] and PAYMENT PLANS, and
// SMART CONTRACTS, and PURSEs into these functions, and they should be able
// to handle any of those types.

int32_t CmdDeposit::depositCheque(
    const string& server,
    const string& myacct,
    const string& mynym,
    const string& instrument,
    string* pOptionalOutput /*=nullptr*/) const
{
    return opentxs::cli::RecordList::depositCheque(
        server, myacct, mynym, instrument, pOptionalOutput);
}

int32_t CmdDeposit::depositPurse(
    const string& server,
    const string& myacct,
    const string& mynym,
    string instrument,
    const string& indices,
    string* pOptionalOutput /*=nullptr*/) const
{
#if OT_CASH
    return OT::App().API().Cash().deposit_purse(
        server, myacct, mynym, instrument, indices, pOptionalOutput);
#else
    return -1;
#endif  // OT_CASH
}
