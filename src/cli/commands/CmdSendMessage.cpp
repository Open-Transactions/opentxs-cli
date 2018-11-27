// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdSendMessage.hpp"

#include <opentxs/opentxs.hpp>

#define OT_METHOD "opentxs::CmdSendMessage::"

namespace opentxs
{
CmdSendMessage::CmdSendMessage()
{
    command = "sendmessage";
    args[0] = "--mynym <nym>";
    args[1] = "--hisnym <nym>";
    args[2] = "--server [<server>]";
    category = catOtherUsers;
    help = "Send a message to hisnym's in-mail (using nym or contact id).";
    usage = "If server is specified, hisnym must be a nym.  If server is "
            "not specified, hisnym must be a contact id.";
}

std::int32_t CmdSendMessage::contact(
    const std::string& mynym,
    const std::string& hisnym,
    const std::string& message)
{
    const OTIdentifier nymID = Identifier::Factory(mynym);
    const OTIdentifier contactID = Identifier::Factory(hisnym);
    auto& thread = Opentxs::Client().UI().ActivityThread(nymID, contactID);
    const auto loaded = thread.SetDraft(message);

    if (false == loaded) {
        LogOutput(OT_METHOD)(__FUNCTION__)(
            ": Failed to load message.").Flush();
        return -1;
    }

    const auto sent = thread.SendDraft();

    if (false == sent) {
        LogOutput(OT_METHOD)(__FUNCTION__)(
            ": Error sending message.").Flush();
        return -1;
    }

    return 0;
}

std::int32_t CmdSendMessage::nym(
    const std::string& server,
    const std::string& mynym,
    std::string& hisnym,
    const std::string& message)
{
    if (!checkNym("hisnym", hisnym)) {
        LogOutput(OT_METHOD)(__FUNCTION__)(
            ": Bad recipient.").Flush();

        return -1;
    }

    std::string response;
    {
        response = Opentxs::
                       Client()
                       .ServerAction()
                       .SendMessage(
                           Identifier::Factory(mynym),
                           Identifier::Factory(server),
                           Identifier::Factory(hisnym),
                           message)
                       ->Run();
    }

    return processResponse(response, "send message");
}

std::int32_t CmdSendMessage::run(
    std::string mynym,
    std::string hisnym,
    std::string server)
{
    const bool haveServer = checkServer("server", server);

    if (!checkNym("mynym", mynym)) {
        LogOutput(OT_METHOD)(__FUNCTION__)(": Bad sender.").Flush();

        return -1;
    }

    std::string input = inputText("your message");

    if ("" == input) {
        LogOutput(OT_METHOD)(__FUNCTION__)(": Bad message.").Flush();

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
