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

#include "CmdShowIssuers.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{

CmdShowIssuers::CmdShowIssuers()
{
    command = "showissuers";
    args[0] = "--mynym <nym>";
    args[1] = "--currency <currency>";
    category = catAccounts;
    help = "Show the issuer list and associated accounts for a nym in the wallet.";
}

std::int32_t CmdShowIssuers::runWithOptions()
{
    return run(getOption("mynym"), getOption("currency"));
}

void CmdShowIssuers::print_accounts(const opentxs::ui::IssuerItem& issuer) const
{
    auto account = issuer.First();
    auto lastAccount = account->Last();

    if (false == account->Valid()) { return; }

    print_line(account.get());

    while (false == lastAccount) {
        account = issuer.Next();
        lastAccount = account->Last();
        print_line(account.get());
    }
}

void CmdShowIssuers::print_line(const opentxs::ui::IssuerItem& line) const
{
    otOut << "* " << line.Name() << " [";

    if (line.ConnectionState()) {
        otOut << "*";
    } else {
        otOut << " ";
    }

    otOut << "]\n";
    print_accounts(line);
}

void CmdShowIssuers::print_line(
    const opentxs::ui::AccountSummaryItem& line) const
{
    otOut << " * " << line.Name() << " " << line.DisplayBalance() << "\n";
}

std::int32_t CmdShowIssuers::run(
    std::string mynym,
    std::string currency)
{
    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    if (!checkMandatory("currency", currency)) {
        return -1;
    }

    const auto currencyType = proto::ContactItemType(std::stoi(currency));
    const OTIdentifier nymID = Identifier::Factory(mynym);
    auto& list = OT::App().UI().AccountSummary(nymID, currencyType);
    otOut << "Issuers:\n";
    dashLine();
    auto issuer = list.First();

    if (false == issuer->Valid()) {

        return 1;
    }

    auto lastIssuer = issuer->Last();
    print_line(issuer.get());

    while (false == lastIssuer) {
        issuer = list.Next();
        lastIssuer = issuer->Last();
        print_line(issuer.get());
    }

    otOut << std::endl;

    return 1;
}
}  // namespace opentxs
