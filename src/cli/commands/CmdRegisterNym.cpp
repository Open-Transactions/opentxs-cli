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

#include "CmdRegisterNym.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{

CmdRegisterNym::CmdRegisterNym()
{
    command = "registernym";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "[--publish <true|FALSE>]";
    args[3] = "[--primary <true|FALSE>]";
    category = catAdmin;
    help = "Register mynym onto an OT server.";
    usage = "Use --publish to set the server as mynym's preferred server and "
            "--primary to force it to replace the current one.";
}

std::int32_t CmdRegisterNym::runWithOptions()
{
    return run(
        getOption("server"),
        getOption("mynym"),
        getOption("publish"),
        getOption("primary"));
}

std::int32_t CmdRegisterNym::run(
    std::string server,
    std::string mynym,
    std::string publish,
    std::string primary)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    bool shouldPublish{false};
    if ("" != publish && !checkBoolean("publish", publish)) {
        return -1;
    } else {
        shouldPublish = publish == "true";
    }
    if (!checkNym("mynym", mynym)) { return -1; }
    bool isPrimary{false};
    if ("" != primary && !checkBoolean("primary", primary)) {
         return -1;
    } else {
        isPrimary = primary == "true";
    }
    if (!shouldPublish && isPrimary) {
        otOut << "Can't make the server primary if it isn't published.";

        return -1;
    }

    auto& sync = OT::App().API().Sync();

    OTIdentifier taskID = sync.RegisterNym(
        Identifier::Factory(mynym), Identifier::Factory(server), shouldPublish, isPrimary);

    ThreadStatus status = sync.Status(taskID);
    while (status == ThreadStatus::RUNNING) {
        Log::Sleep(std::chrono::milliseconds(100));
        status = sync.Status(taskID);
    }

    switch (status) {
        case ThreadStatus::FINISHED_SUCCESS: {
            otOut << "Nym registered successfully " << std::endl;
        } break;
        case ThreadStatus::FINISHED_FAILED: {
            otOut << "Nym not registered " << std::endl;
            [[fallthrough]];
        }
        case ThreadStatus::ERROR:
        case ThreadStatus::SHUTDOWN:
        default: {
            return -1;
        }
    }

    return 0;
}
} // namespace opentxs
