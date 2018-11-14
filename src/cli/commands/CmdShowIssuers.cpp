// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdShowIssuers.hpp"

#include <opentxs/opentxs.hpp>

#define OT_METHOD "opentxs::CmdShowIssuers::"

namespace opentxs
{

CmdShowIssuers::CmdShowIssuers()
{
    command = "showissuers";
    args[0] = "--mynym <nym>";
    args[1] = "--currency <currency>";
    category = catAccounts;
    help =
        "Show the issuer list and associated accounts for a nym in the wallet.";
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
    LogNormal(OT_METHOD)(__FUNCTION__)(": * ")(line.Name())(" [").Flush();

    if (line.ConnectionState()) {
        LogNormal(OT_METHOD)(__FUNCTION__)("*").Flush();
    } else {
        LogNormal(OT_METHOD)(__FUNCTION__)(" ").Flush();
    }

    LogNormal(OT_METHOD)(__FUNCTION__)("]").Flush();
    print_accounts(line);
}

void CmdShowIssuers::print_line(
    const opentxs::ui::AccountSummaryItem& line) const
{
    LogNormal(OT_METHOD)(__FUNCTION__)(" * ")(line.Name())(" ")(
        line.DisplayBalance())
        .Flush();
}

std::int32_t CmdShowIssuers::run(std::string mynym, std::string currency)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    if (!checkMandatory("currency", currency)) { return -1; }

    const auto currencyType = proto::ContactItemType(std::stoi(currency));
    const OTIdentifier nymID = Identifier::Factory(mynym);
    auto& list = Opentxs::Client().UI().AccountSummary(nymID, currencyType);
    LogNormal(OT_METHOD)(__FUNCTION__)(":Issuers: ").Flush();
    dashLine();
    auto issuer = list.First();

    if (false == issuer->Valid()) { return 1; }

    auto lastIssuer = issuer->Last();
    print_line(issuer.get());

    while (false == lastIssuer) {
        issuer = list.Next();
        lastIssuer = issuer->Last();
        print_line(issuer.get());
    }
    LogNormal(OT_METHOD)(__FUNCTION__)(" ").Flush();

    return 1;
}
}  // namespace opentxs
