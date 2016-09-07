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

#include "CmdInmail.hpp"

#include "CmdBase.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/OTAPI_Wrap.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdInmail::CmdInmail()
{
    command = "inmail";
    args[0] = "--mynym <nym>";
    args[1] = "[--index <index>]";
    category = catOtherUsers;
    help = "Show mynym's in-mail";
    usage = "Omitting --index shows all in-mail.";
}

CmdInmail::~CmdInmail()
{
}

int32_t CmdInmail::runWithOptions()
{
    return run(getOption("mynym"), getOption("index"));
}

int32_t CmdInmail::run(string mynym, string index)
{
    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    int32_t items = OTAPI_Wrap::GetNym_MailCount(mynym);
    if (0 > items) {
        otOut << "Error: cannot load mail inbox item count.\n";
        return -1;
    }

    if (0 == items) {
        otOut << "The mail inbox is empty.\n";
        return 0;
    }

    // all items?
    if ("" == index) {
        cout << "Mail inbox contents:\n\n";

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

bool CmdInmail::showMail(const string& mynym, int32_t index,
                         bool showContents) const
{
    dashLine();

    if (!OTAPI_Wrap::Nym_VerifyMailByIndex(mynym, index)) {
        cout << "UNVERIFIED inmail message " << index << ".\n";
    }
    else {
        cout << "Verified inmail message " << index << "\n";
    }

    string nymId = OTAPI_Wrap::GetNym_MailSenderIDByIndex(mynym, index);
    if ("" != nymId) {
        string name = OTAPI_Wrap::GetNym_Name(nymId);
        cout << "Mail from: " << nymId << " \"" << name << "\" \n";
    }

    string server = OTAPI_Wrap::GetNym_MailNotaryIDByIndex(mynym, index);
    if ("" != server) {
        string name = OTAPI_Wrap::GetServer_Name(server);
        cout << "At server: " << server << " \"" << name << "\" \n";
    }

    if (showContents) {
        string contents = OTAPI_Wrap::GetNym_MailContentsByIndex(mynym, index);
        cout << "Contents:\n" << contents << "\n\n";
    }

    return true;
}
