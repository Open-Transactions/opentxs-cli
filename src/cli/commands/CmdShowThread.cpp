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

#include "CmdShowThread.hpp"

#include <opentxs/api/client/Wallet.hpp>
#include <opentxs/api/storage/Storage.hpp>
#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/client/OT_API.hpp>
#include <opentxs/core/Identifier.hpp>
#include <opentxs/core/Log.hpp>
#include <opentxs/core/Message.hpp>
#include <opentxs/core/String.hpp>
#include <opentxs/OT.hpp>
#include <opentxs/Types.hpp>

namespace opentxs
{
CmdShowThread::CmdShowThread()
{
    command = "showthread";
    args[0] = "--mynym <nym>";
    args[1] = "--thread <threadID>";
    category = catOtherUsers;
    help = "Display the contents of an activity thread.";
}

std::int32_t CmdShowThread::runWithOptions()
{
    return run(getOption("mynym"), getOption("thread"));
}

std::int32_t CmdShowThread::run(
    std::string mynym,
    const std::string& threadID)
{
    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    auto& ot = OT::App();
    auto& storage = ot.DB();
    auto& wallet = ot.Wallet();
    std::shared_ptr<proto::StorageThread> thread;
    storage.Load(mynym, threadID, thread);

    if (!thread) {
        otErr << "Thread does not exist." << std::endl;

        return -1;
    }

    otOut << "Thread " << thread->id() << ", version " << thread->version()
          << "\nParticipants:\n" << "    * " << mynym
          << " (you)\n";

          for (const auto& it : thread->participant()) {
              otOut << "    * " << it << "\n";
          }

    otOut << "Contents (" << thread->item().size() << " items):\n";

    for (const auto& item : thread->item()) {
        const StorageBox box = static_cast<StorageBox>(item.box());
        otOut << "-----------------------------------------------------------\n"
              << "Item " << item.id() << ", version " << item.version() << "\n"
              << "Index: " << item.index() << "\n"
              << "Time: " << item.time() << "\n"
              << "Box: ";

        switch (box) {
            case (StorageBox::MAILINBOX) : {
                otOut << "mail inbox\n";
            } break;
            case (StorageBox::MAILOUTBOX) : {
                otOut << "mail outbox\n";
            } break;
            case (StorageBox::SENTPEERREQUEST) :
            case (StorageBox::INCOMINGPEERREQUEST) :
            case (StorageBox::SENTPEERREPLY) :
            case (StorageBox::INCOMINGPEERREPLY) :
            case (StorageBox::FINISHEDPEERREQUEST) :
            case (StorageBox::FINISHEDPEERREPLY) :
            case (StorageBox::PROCESSEDPEERREQUEST) :
            case (StorageBox::PROCESSEDPEERREPLY) :
            default : {}
        }
        otOut << "Account: " << item.account() << "\n";

        if (item.unread()) { otOut << "Unread\n"; }

        auto message = ot.API().OTAPI().BoxContents(
            Identifier(mynym), Identifier(item.id()), box);

        if (message.empty()) {
            otErr << "Failed to load message.\n";

            continue;
        }

        otOut << "Contents:\n";
        otOut << message;
    }

    otOut << std::endl;

    return 0;
}
} // namespace opentxs
