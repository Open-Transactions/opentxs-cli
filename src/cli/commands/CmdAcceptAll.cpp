// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdAcceptAll.hpp"

#include <opentxs/opentxs.hpp>

#include <cstdint>
#include <string>

using namespace opentxs;
using namespace std;

CmdAcceptAll::CmdAcceptAll()
{
    command = "acceptall";
    args[0] = "--myacct <account>";
    args[1] = "[--server <server>]";
    category = catAccounts;
    help = "Accept all incoming transfers, receipts, payments, invoices.";
    usage = "Server is an Opentxs notary where you receive incoming messages "
            "(including payment instruments). Account is where you wish to "
            "deposit any cheques (etc) that are found. There's no guarantee "
            "that the account is on the same server where the messages are "
            "coming from. Unless you don't specify a server, of course, in "
            "which case it will use the account's server Id. But there's also "
            "no guarantee that the payment instrument is drawn on an account "
            "located on the same notary where your account is located. It's "
            "YOUR responsibility to make sure you pick the correct server and "
            "the correct account, which is basically impossible in the case of "
            "'ALL'. (Which this is). Therefore this command is well-nigh "
            "useless. However, it could be easily updated to simply determine "
            "on its own which account is the correct one for each instrument, "
            "and to create an account if an appropriate one can't be found. "
            "Justus has a new function that does this, so we could just call "
            "that here, so it happens in the background. TODO HERE.";
}

CmdAcceptAll::~CmdAcceptAll() {}

std::int32_t CmdAcceptAll::runWithOptions()
{
    return run(getOption("server"), getOption("myacct"));
}

std::int32_t CmdAcceptAll::run(string server, string myacct)
{
    if (!checkAccount("myacct", myacct)) { return -1; }
    if (!checkServer("server", server)) {
        server = SwigWrap::GetAccountWallet_NotaryID(myacct);
    }
    if (!checkServer("server", server)) { return -1; }
    string& transport_notary = server;

    //  Incoming transfers and receipts (asset account inbox.)
    bool success = 0 <= acceptFromInbox(myacct, "all", opentxs::cli::RecordList::typeBoth);

    // Incoming payments -- cheques, purses, vouchers
    success &=
        0 <= acceptFromPaymentbox(transport_notary, myacct, "all", "PURSE");
    success &=
        0 <= acceptFromPaymentbox(transport_notary, myacct, "all", "CHEQUE");

    // Invoices LAST
    // So the MOST money is in the account before it starts paying out.
    success &=
        0 <= acceptFromPaymentbox(transport_notary, myacct, "all", "INVOICE");

    return success ? 1 : -1;
}
