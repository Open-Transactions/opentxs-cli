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

#include "CmdBaseInstrument.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/OTAPI_Wrap.hpp>
#include <opentxs/client/OT_ME.hpp>
#include <opentxs/client/MadeEasy.hpp>
#include <opentxs/core/app/App.hpp>
#include <opentxs/core/app/Api.hpp>
#include <opentxs/core/util/Common.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <ostream>
#include <string>
#include <vector>

using namespace opentxs;
using namespace std;

CmdBaseInstrument::CmdBaseInstrument()
{
}

CmdBaseInstrument::~CmdBaseInstrument()
{
}

int32_t CmdBaseInstrument::getTokens(vector<string>& tokens,
                                     const string& server, const string& mynym,
                                     const string& assetType, string purse,
                                     const string& indices) const
{

    if ("" == indices) {
        return 1;
    }

    int32_t items = OTAPI_Wrap::Purse_Count(server, assetType, purse);
    if (0 > items) {
        otOut << "Error: cannot load purse item count.\n";
        return -1;
    }

    if (1 > items) {
        otOut << "Error: The purse is empty.\n";
        return -1;
    }

    bool all = "all" == indices;
    for (int32_t i = 0; i < items; i++) {
        string token = OTAPI_Wrap::Purse_Peek(server, assetType, mynym, purse);
        if ("" == token) {
            otOut << "Error:cannot load token from purse.\n";
            return -1;
        }

        purse = OTAPI_Wrap::Purse_Pop(server, assetType, mynym, purse);
        if ("" == purse) {
            otOut << "Error: cannot load updated purse.\n";
            return -1;
        }

        string tokenID = OTAPI_Wrap::Token_GetID(server, assetType, token);
        if ("" == tokenID) {
            otOut << "Error: cannot get token ID.\n";
            return -1;
        }

        if (!all && OTAPI_Wrap::NumList_VerifyQuery(indices, to_string(i))) {
            tokens.push_back(tokenID);
        }
    }

    return 1;
}

int32_t CmdBaseInstrument::sendPayment(const string& cheque, string sender,
                                       const char* what) const
{
    string server = OTAPI_Wrap::Instrmnt_GetNotaryID(cheque);
    if ("" == server) {
        otOut << "Error: cannot get server.\n";
        return -1;
    }

    if ("" == sender) {
        sender = OTAPI_Wrap::Instrmnt_GetSenderNymID(cheque);
        if ("" == sender) {
            otOut << "Error: cannot get sender.\n";
            return -1;
        }
    }

    string recipient = OTAPI_Wrap::Instrmnt_GetRecipientNymID(cheque);
    if ("" == recipient) {
        otOut << "Error: cannot get recipient.\n";
        return -1;
    }

     
    string response =
        OT_ME::It().send_user_payment(server, sender, recipient, cheque);
    return processResponse(response, what);
}

string CmdBaseInstrument::writeCheque(string myacct, string hisnym,
                                      string amount, string memo,
                                      string validfor, bool isInvoice) const
{
    if (!checkAccount("myacct", myacct)) {
        return "";
    }

    if ("" != hisnym && !checkNym("hisnym", hisnym)) {
        return "";
    }

    int64_t value = checkAmount("amount", amount, myacct);
    if (OT_ERROR_AMOUNT == value) {
        return "";
    }

    if ("" != validfor && !checkValue("validfor", validfor)) {
        return "";
    }

    string server = OTAPI_Wrap::GetAccountWallet_NotaryID(myacct);
    if ("" == server) {
        otOut << "Error: cannot determine server from myacct.\n";
        return "";
    }

    string mynym = OTAPI_Wrap::GetAccountWallet_NymID(myacct);
    if ("" == mynym) {
        otOut << "Error: cannot determine mynym from myacct.\n";
        return "";
    }

    // make sure we can access the public key before trying to write a cheque
    if ("" != hisnym) {
        if (App::Me().API().ME().load_or_retrieve_encrypt_key(server, mynym, hisnym) ==
            "") {
            otOut << "Error: cannot load public key for hisnym.\n";
            return "";
        }
    }

     
    if (!OT_ME::It().make_sure_enough_trans_nums(10, server, mynym)) {
        otOut << "Error: cannot reserve transaction numbers.\n";
        return "";
    }

    int64_t oneMonth = OTTimeGetSecondsFromTime(OT_TIME_MONTH_IN_SECONDS);
    int64_t timeSpan = "" != validfor ? stoll(validfor) : oneMonth;
    time64_t from = OTAPI_Wrap::GetTime();
    time64_t until = OTTimeAddTimeInterval(from, timeSpan);

    string cheque =
        OTAPI_Wrap::WriteCheque(server, isInvoice ? -value : value, from, until,
                                myacct, mynym, memo, hisnym);
    if ("" == cheque) {
        otOut << "Error: cannot write cheque.\n";
        return "";
    }

    // Record it in the records?
    // Update: We wouldn't record that here. Instead, OTAPI_Wrap::WriteCheque
    // should drop a notice into the payments outbox, the same as it does when
    // you "sendcheque" (after all, the same resolution would be expected once
    // it is cashed.)

    return cheque;
}
