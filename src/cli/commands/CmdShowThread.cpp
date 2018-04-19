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

#include <opentxs/opentxs.hpp>

#include <ctime>

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

std::int32_t CmdShowThread::run(std::string mynym, const std::string& threadID)
{
    if (!checkNym("mynym", mynym)) {

        return -1;
    }

    const auto& thread =
        OT::App().UI().ActivityThread(Identifier(mynym), Identifier(threadID));
    const auto& first = thread.First();
    otOut << thread.DisplayName() << "\n";

    if (false == first.Valid()) {
        return 0;
    }

    auto last = first.Last();
    otOut << " * " << time(first.Timestamp()) << " " << first.Text() << "\n";

    while (false == last) {
        auto& line = thread.Next();
        last = line.Last();
        otOut << " * " << time(line.Timestamp()) << " " << line.Text() << "\n";
    }

    otOut << std::endl;

    return 0;
}

std::string CmdShowThread::time(
    const std::chrono::system_clock::time_point in) const
{
    const auto converted = std::chrono::system_clock::to_time_t(in);

    return std::ctime(&converted);
}
}  // namespace opentxs
