// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdAssignBitcoinAddress.hpp"

#include <opentxs/opentxs.hpp>

#define OT_METHOD "opentxs::CmdAssignBitcoinAddress::"

namespace opentxs
{
CmdAssignBitcoinAddress::CmdAssignBitcoinAddress()
{
    command = "assignbitcoinaddress";
    args[0] = "--mynym <nym>";
    args[1] = "--account <account ID>";
    args[2] = "--index <address index>";
    args[3] = "--contact [<contact ID>]";
    args[4] = "--chain [<internal or external>]";
    category = catBlockchain;
    help = "Assign a contact ID to a bitcoin address";
}

std::int32_t CmdAssignBitcoinAddress::runWithOptions()
{
    return run(
        getOption("mynym"),
        getOption("account"),
        getOption("chain"),
        getOption("index"),
        getOption("contact"));
}

std::int32_t CmdAssignBitcoinAddress::run(
    std::string mynym,
    const std::string& account,
    const std::string& chain,
    const std::string& indexArg,
    const std::string& contact)
{
    if (false == checkNym("mynym", mynym)) { return -1; }

    OTIdentifier accountID = Identifier::Factory(account);

    if (accountID->empty()) {
        LogOutput(OT_METHOD)(__FUNCTION__)(": Invalid account ID.")
              .Flush();

        return -1;
    }

    BIP44Chain change{EXTERNAL_CHAIN};

    if ("internal" == chain) { change = INTERNAL_CHAIN; }

    std::uint32_t index{0};

    try {
        if (sizeof(std::int32_t) == sizeof(int)) {
            auto temp = std::stoi(indexArg);

            if (0 <= temp) {
                index = temp;
            } else {
                LogOutput(OT_METHOD)(__FUNCTION__)(": Negative index.")
                      .Flush();

                return -1;
            }
        } else if (sizeof(std::int32_t) == sizeof(unsigned long)) {
            index = std::stoul(indexArg);
        } else if (sizeof(std::int32_t) == sizeof(unsigned long long)) {
            index = std::stoull(indexArg);
        }
    } catch (const std::invalid_argument&) {
        LogOutput(OT_METHOD)(__FUNCTION__)(": Invalid index.").Flush();

        return -1;
    } catch (const std::out_of_range&) {
        LogOutput(OT_METHOD)(__FUNCTION__)(": Index out of range.")
              .Flush();

        return -1;
    }

    const OTIdentifier nymID = Identifier::Factory(mynym);
    const auto assigned = Opentxs::Client().Blockchain().AssignAddress(
        nymID, accountID, index, Identifier::Factory(contact), change);

    if (false == assigned) {
        LogOutput(OT_METHOD)(__FUNCTION__)(": Failed to allocate address.")
              .Flush();

        return -1;
    }

    const auto address = Opentxs::Client().Blockchain().LoadAddress(
        nymID, accountID, index, change);

    if (false == bool(address)) {
        LogOutput(OT_METHOD)(__FUNCTION__)(": Failed to load address.")
              .Flush();

        return -1;
    }

    LogNormal(OT_METHOD)(__FUNCTION__)(": Assigned address at index ")(
       address->index())(": ")(address->address())(".")
        .Flush();
    LogNormal(OT_METHOD)(__FUNCTION__)(": (")(address->label())(
        " ) to contact: ")(address->contact())(".")
        .Flush();

    return 0;
}
}  // namespace opentxs
