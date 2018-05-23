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

#include "CmdShowAccount.hpp"

#include <opentxs/opentxs.hpp>

#include <cstdint>
#include <iostream>
#include <string>

namespace opentxs
{
CmdShowAccount::CmdShowAccount()
{
    command = "showaccount";
    args[0] = "--mynym <nym id>";
    args[1] = "--myacct <account id>";
    category = catAccounts;
    help = "Show myacct's stats.";
}

void CmdShowAccount::display_row(const ui::BalanceItem& row) const
{
    const auto time = std::chrono::system_clock::to_time_t(row.Timestamp());
    otOut << " " << row.Text() << " " << row.DisplayAmount() << " "
          << std::ctime(&time) << "\n " << row.Memo() << "\n";
}

std::int32_t CmdShowAccount::runWithOptions() {

    return run(getOption("mynym"), getOption("myacct"));
}

std::int32_t CmdShowAccount::run(std::string mynym, std::string myacct)
{
    if (!checkNym("mynym", mynym)) {

        return -1;
    }

    if (!checkAccount("myacct", myacct)) {

        return -1;
    }

    const auto nymID = Identifier::Factory(mynym);
    const auto accountID = Identifier::Factory(myacct);
    auto& list = OT::App().UI().AccountActivity(nymID, accountID);
    otOut << "Account " << myacct << ":\n";
    dashLine();
    auto& firstRow = list.First();

    if (firstRow.Valid()) {
        display_row(firstRow);
        auto last = firstRow.Last();

        while (false == last) {
            auto& row = list.Next();
            display_row(row);
            last = row.Last();
        }
    }

    otOut << std::endl;

    return 1;
}
} // namespace opentxs
