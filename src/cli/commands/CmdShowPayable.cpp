// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdShowPayable.hpp"

#include <opentxs/opentxs.hpp>

#define OT_METHOD "opentxs::CmdShowPayable"

namespace opentxs
{

CmdShowPayable::CmdShowPayable()
{
    command = "showpayable";
    args[0] = "--mynym <nym>";
    args[1] = "--currency <id>";
    category = catOtherUsers;
    help = "Show the list of payable contacts for a currency.";
}

std::int32_t CmdShowPayable::runWithOptions()
{
    return run(getOption("mynym"), getOption("currency"));
}

std::int32_t CmdShowPayable::run(std::string mynym, std::string currency)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    if (!checkMandatory("currency", currency)) { return -1; }

    auto currencyType = proto::ContactItemType(std::stoi(currency));

    const OTIdentifier nymID = Identifier::Factory({mynym});
    auto& list = Opentxs::Client().UI().PayableList(nymID, currencyType);
    LogNormal(OT_METHOD)(__FUNCTION__)(": Contacts:").Flush();
    dashLine();
    auto line = list.First();
    auto last = line->Last();

    if (false == line->Valid()) { return 1; }
    {

        LogNormal(OT_METHOD)(__FUNCTION__)(":  ")(line->Section())(" ")(
            line->DisplayName())(" (")(line->ContactID())(")")
            .Flush();
    }
    while (false == last) {
        line = list.Next();
        last = line->Last();

        if (false == line->Valid()) { return 1; }
        {
            LogNormal(OT_METHOD)(__FUNCTION__)(": ")(line->Section())("  ")(
                line->DisplayName())(" (")(line->ContactID())(")")
                .Flush();
        }
    }

    LogNormal(OT_METHOD)(__FUNCTION__)(" ").Flush();

    return 1;
}
}  // namespace opentxs
