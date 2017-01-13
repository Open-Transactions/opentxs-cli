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

#include "CmdSendMessage.hpp"

#include "CmdBase.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/MadeEasy.hpp>
#include <opentxs/core/app/App.hpp>
#include <opentxs/core/app/Api.hpp>
#include <opentxs/core/Log.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdSendMessage::CmdSendMessage()
{
    command = "sendmessage";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--hisnym <nym>";
    category = catOtherUsers;
    help = "Send a message to hisnym's in-mail.";
}

CmdSendMessage::~CmdSendMessage()
{
}

int32_t CmdSendMessage::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("hisnym"));
}

int32_t CmdSendMessage::run(string server, string mynym, string hisnym)
{
    if (!checkServer("server", server)) {
        return -1;
    }

    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    if (!checkNym("hisnym", hisnym)) {
        return -1;
    }

    // make sure we can access the public keys before trying to send a message

    if ("" == App::Me().API().ME().load_or_retrieve_encrypt_key(server, mynym, mynym)) {
        otOut << "Error: cannot load public key for mynym.\n";
        return -1;
    }

    if ("" == App::Me().API().ME().load_or_retrieve_encrypt_key(server, mynym, hisnym)) {
        otOut << "Error: cannot load public key for hisnym.\n";
        return -1;
    }

    string input = inputText("your message");
    if ("" == input) {
        return -1;
    }

    string response = App::Me().API().ME().send_user_msg(server, mynym, hisnym, input);
    return processResponse(response, "send message");
}
