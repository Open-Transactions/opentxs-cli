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

#include <opentxs/api/client/ServerAction.hpp>
#include <opentxs/api/client/Sync.hpp>
#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/client/ServerAction.hpp>
#include <opentxs/core/Identifier.hpp>
#include <opentxs/core/Log.hpp>
#include <opentxs/OT.hpp>

namespace opentxs
{
CmdSendMessage::CmdSendMessage()
{
    command = "sendmessage";
    args[0] = "--mynym <nym>";
    args[1] = "--hisnym <nym>";
    args[2] = "--server [<server>]";
    category = catOtherUsers;
    help = "Send a message to hisnym's in-mail.";
}

std::int32_t CmdSendMessage::contact(
    const std::string& mynym,
    const std::string& hisnym,
    const std::string& message)
{
    auto result = OT::App().API().Sync().MessageContact(
        Identifier(mynym), Identifier(hisnym), message);

    otErr << "Thread " << String(result) << " started." << std::endl;

    return String(result).Exists();
}

std::int32_t CmdSendMessage::nym(
    const std::string& server,
    const std::string& mynym,
    std::string& hisnym,
    const std::string& message)
{
    if (!checkNym("hisnym", hisnym)) {
        otErr << "Bad recipient" << std::endl;

        return -1;
    }

    auto& se = OT::App().API().ServerAction();
    std::string response = se.SendMessage(
                                 Identifier(mynym),
                                 Identifier(server),
                                 Identifier(hisnym),
                                 message)
                               ->Run();

    return processResponse(response, "send message");
}

std::int32_t CmdSendMessage::run(
    std::string mynym,
    std::string hisnym,
    std::string server)
{
    const bool haveServer = checkServer("server", server);

    if (!checkNym("mynym", mynym)) {
        otErr << "Bad sender" << std::endl;

        return -1;
    }

    std::string input = inputText("your message");

    if ("" == input) {
        otErr << "Bad message" << std::endl;

        return -1;
    }

    if (haveServer) {
        return nym(server, mynym, hisnym, input);
    } else {
        return contact(mynym, hisnym, input);
    }
}

std::int32_t CmdSendMessage::runWithOptions()
{
    return run(getOption("mynym"), getOption("hisnym"), getOption("server"));
}
}  // namespace opentxs
