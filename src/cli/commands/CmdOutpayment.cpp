// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdOutpayment.hpp"

#include <opentxs/opentxs.hpp>

#include <iostream>
#include <stdint.h>
#include <string>

#define OT_METHOD "opentxs::CmdOutpayment::"

using namespace opentxs;
using namespace std;

CmdOutpayment::CmdOutpayment()
{
    command = "outpayment";
    args[0] = "--mynym <nym>";
    args[1] = "[--index <index>]";
    category = catOtherUsers;
    help = "Show mynym's outgoing payments box.";
    usage = "Omitting --index shows all outpayments.";
}

CmdOutpayment::~CmdOutpayment() {}

int32_t CmdOutpayment::runWithOptions()
{
    return run(getOption("mynym"), getOption("index"));
}

int32_t CmdOutpayment::run(string mynym, string index)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    int32_t items = SwigWrap::GetNym_OutpaymentsCount(mynym);
    if (0 > items) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot load payment outbox item count.")
            .Flush();
        return -1;
    }

    if (0 == items) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": The payment outbox is empty.")
            .Flush();
        return 0;
    }

    // all items?
    if ("" == index) {
        cout << "Payment outbox contents:\n\n";

        int32_t retVal = 1;
        for (int32_t i = 0; i < items; i++) {
            if (!showOutpayment(mynym, i, false)) {
                LogNormal(OT_METHOD)(__FUNCTION__)(
                    ": Error: cannot retrieve outpayment ")(i)(".")
                    .Flush();
                retVal = -1;
            }
        }

        return retVal;
    }

    int32_t messageNr = checkIndex("index", index, items);
    if (0 > messageNr) { return -1; }

    if (!showOutpayment(mynym, messageNr, true)) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot retrieve outpayment ")(messageNr)(".")
            .Flush();
        return -1;
    }

    return 1;
}

bool CmdOutpayment::showOutpayment(
    const string& mynym,
    int32_t index,
    bool showMessage)
{
    string payment = SwigWrap::GetNym_OutpaymentsContentsByIndex(mynym, index);
    if ("" == payment) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot load payment ")(
            index)(".")
            .Flush();
        return false;
    }

    dashLine();

    if (!SwigWrap::Nym_VerifyOutpaymentsByIndex(mynym, index)) {
        cout << "UNVERIFIED outpayment message " << index << ".\n";
    } else {
        cout << "Verified outpayment message " << index << "\n";
    }

    string server = SwigWrap::GetNym_OutpaymentsNotaryIDByIndex(mynym, index);
    string recipient =
        SwigWrap::GetNym_OutpaymentsRecipientIDByIndex(mynym, index);
    int64_t amount = SwigWrap::Instrmnt_GetAmount(payment);
    string instrumentDefinitionID =
        SwigWrap::Instrmnt_GetInstrumentDefinitionID(payment);
    string type = SwigWrap::Instrmnt_GetType(payment);

    cout << "         Amount: ";
    if (OT_ERROR_AMOUNT == amount) {
        cout << "UNKNOWN_PAYMENT_AMOUNT\n";
    } else {
        cout << amount;
        if ("" == type) { type = "UNKNOWN_PAYMENT_TYPE"; }
        cout << "  (" << type << ": "
             << SwigWrap::FormatAmount(instrumentDefinitionID, amount) << ")";
        cout << "\n";
    }

    cout << "  Of instrument definition: " << instrumentDefinitionID;
    if ("" != instrumentDefinitionID) {
        cout << " \"" << SwigWrap::GetAssetType_Name(instrumentDefinitionID)
             << "\"";
    }
    cout << "\n";

    if ("" != recipient) {
        cout << "Payment sent to: " << recipient;
        cout << " \"" << SwigWrap::GetNym_Name(recipient) << "\" ";
    }
    cout << "\n";

    if ("" != server) {
        cout << "   At server ID: " << server;
        cout << " \"" << SwigWrap::GetServer_Name(server) << "\" ";
    }
    cout << "\n";

    if (showMessage) { cout << "     Instrument: \n" << payment << "\n"; }

    return true;
}
