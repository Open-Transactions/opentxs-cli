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

#include "CmdShowPayment.hpp"

#include "CmdBase.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/OTAPI_Wrap.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdShowPayment::CmdShowPayment()
{
    command = "showpayment";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "[--index <index>]";
    args[3] = "[--showmemo <true|false>]";
    category = catOtherUsers;
    help = "Show details of an incoming payment in the payments inbox.";
    usage = "Default index is 0. Default showmemo is false.";
}

CmdShowPayment::~CmdShowPayment()
{
}

int32_t CmdShowPayment::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("index"),
               getOption("showmemo"));
}

int32_t CmdShowPayment::run(string server, string mynym, string index,
                            string showmemo)
{
    if (!checkServer("server", server)) {
        return -1;
    }

    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    if ("" != showmemo && !checkFlag("showmemo", showmemo)) {
        return -1;
    }

    string inbox = OTAPI_Wrap::LoadPaymentInbox(server, mynym);
    if ("" == inbox) {
        otOut << "Error: cannot load payment inbox.\n";
        return -1;
    }

    int32_t items = OTAPI_Wrap::Ledger_GetCount(server, mynym, mynym, inbox);
    if (0 > items) {
        otOut << "Error: cannot load payment inbox item count.\n";
        return -1;
    }

    int32_t messageNr = "" == index ? 0 : checkIndex("index", index, items);
    if (0 > messageNr) {
        return -1;
    }

    if (0 == items) {
        otOut << "The payment inbox is empty.\n";
        return 0;
    }

    cout << "Idx  Amt   Type       Txn#   Memo\n";
    cout << "---------------------------------\n";

    string payment = OTAPI_Wrap::Ledger_GetInstrument(server, mynym, mynym,
                                                      inbox, messageNr);
    if ("" == payment) {
        otOut << "Error: cannot load payment.\n";
        return -1;
    }

    string tx = OTAPI_Wrap::Ledger_GetTransactionByIndex(server, mynym, mynym,
                                                         inbox, messageNr);
    int64_t txNum = OTAPI_Wrap::Ledger_GetTransactionIDByIndex(
        server, mynym, mynym, inbox, messageNr);
    /* int64_t refNum = */ OTAPI_Wrap::Transaction_GetDisplayReferenceToNum(
        server, mynym, mynym, tx);
    int64_t amount = OTAPI_Wrap::Instrmnt_GetAmount(payment);
    string type = OTAPI_Wrap::Instrmnt_GetType(payment);
    string assetType = OTAPI_Wrap::Instrmnt_GetInstrumentDefinitionID(payment);
    string senderUser = OTAPI_Wrap::Instrmnt_GetSenderNymID(payment);
    string senderAcct = OTAPI_Wrap::Instrmnt_GetSenderAcctID(payment);
    string recipientUser = OTAPI_Wrap::Instrmnt_GetRecipientNymID(payment);
    string recipientAcct = OTAPI_Wrap::Instrmnt_GetRecipientAcctID(payment);
    string remitterUser = OTAPI_Wrap::Instrmnt_GetRemitterNymID(payment);
    string remitterAcct = OTAPI_Wrap::Instrmnt_GetRemitterAcctID(payment);

    string memo = OTAPI_Wrap::Instrmnt_GetMemo(payment);
    if ("" != memo) {
        if (memo.find("\n") != string::npos && showmemo != "true") {
            memo = "<too large to display here>";
        }
        else {
            memo = "\"" + memo + "\"";
        }
    }

    string user = "" != remitterUser ? remitterUser : senderUser;
    if ("" != user) {
        string userName = OTAPI_Wrap::GetNym_Name(user);
        if ("" != userName) user += " - \"" + userName + "\"";
        user = "U:" + user + "\n";
    }

    string acct = "" != remitterAcct ? remitterAcct : senderAcct;
    if ("" != acct) {
        string acctName = OTAPI_Wrap::GetAccountWallet_Name(acct);
        if ("" != acctName) acct += " - \"" + acctName + "\"";
        acct = "A:" + acct + "\n";
    }

    cout << messageNr << "    ";

    string fmtAmount = formatAmount(assetType, amount);
    cout << fmtAmount << (fmtAmount.size() < 3 ? "    " : "   ");

    if ("" != assetType) {
        string assetName = OTAPI_Wrap::GetAssetType_Name(assetType);
        if ("" != assetName) assetType += " - \"" + assetName + "\"";
        assetType = "T:" + assetType + "\n";
    }

    cout << type << (type.size() > 10 ? " " : "    ");
    cout << txNum << (to_string(txNum).size() < 2 ? "    " : "   ");
    cout << memo << "\n";
    cout << user;
    cout << acct;
    cout << assetType;
    cout << "Instrument:\n" << payment << "\n";

    return 1;
}
