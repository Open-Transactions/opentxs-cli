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

#include "CmdOutmail.hpp"

#include "CmdBase.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/OTAPI_Wrap.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdOutmail::CmdOutmail()
{
    command = "outmail";
    args[0] = "--mynym <nym>";
    args[1] = "[--index <index>]";
    category = catOtherUsers;
    help = "Show mynym's out-mail.";
    usage = "Omitting --index shows all out-mail.";
}

CmdOutmail::~CmdOutmail()
{
}

int32_t CmdOutmail::runWithOptions()
{
    return run(getOption("mynym"), getOption("index"));
}

int32_t CmdOutmail::run(string mynym, string index)
{
    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    int32_t items = OTAPI_Wrap::GetNym_OutmailCount(mynym);
    if (0 > items) {
        otOut << "Error: cannot load mail outbox item count.\n";
        return -1;
    }

    if (0 == items) {
        otOut << "The mail outbox is empty.\n";
        return 0;
    }

    // all items?
    if ("" == index) {
        cout << "Mail outbox contents:\n\n";

        int32_t retVal = 1;
        for (int32_t i = 0; i < items; i++) {
            if (!showMail(mynym, i, false)) {
                otOut << "Error: cannot retrieve mail message " << i << ".\n";
                retVal = -1;
            }
        }

        return retVal;
    }

    int32_t messageNr = checkIndex("index", index, items);
    if (0 > messageNr) {
        return -1;
    }

    if (!showMail(mynym, messageNr, true)) {
        otOut << "Error: cannot retrieve mail message " << messageNr << ".\n";
        return -1;
    }

    return 1;
}

bool CmdOutmail::showMail(const string& mynym, int32_t index,
                          bool showContents) const
{
    dashLine();

    if (!OTAPI_Wrap::Nym_VerifyOutmailByIndex(mynym, index)) {
        cout << "UNVERIFIED outmail message " << index << ".\n";
    }
    else {
        cout << "Verified outmail message " << index << "\n";
    }

    string nymId = OTAPI_Wrap::GetNym_OutmailRecipientIDByIndex(mynym, index);
    if ("" != nymId) {
        string name = OTAPI_Wrap::GetNym_Name(nymId);
        cout << "Mail to  : " << nymId << " \"" << name << "\" \n";
    }

    string server = OTAPI_Wrap::GetNym_OutmailNotaryIDByIndex(mynym, index);
    if ("" != server) {
        string name = OTAPI_Wrap::GetServer_Name(server);
        cout << "At server: " << server << " \"" << name << "\" \n";
    }

    if (showContents) {
        string contents =
            OTAPI_Wrap::GetNym_OutmailContentsByIndex(mynym, index);
        cout << "Contents:\n" << contents << "\n\n";
    }

    return true;
}
