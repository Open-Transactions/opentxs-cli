// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdAllocateBitcoinAddress.hpp"

#include <opentxs/opentxs.hpp>

#define OT_METHOD "opentxs::CmdAllocateBitcoinAddress::"

namespace opentxs
{
CmdAllocateBitcoinAddress::CmdAllocateBitcoinAddress()
{
    command = "allocatebitcoinaddress";
    args[0] = "--mynym <nym>";
    args[1] = "--account <account ID>";
    args[2] = "--label [<address label>]";
    args[3] = "--chain [<internal or external>]";
    category = catBlockchain;
    help = "Derive bitcoin addresses in a Bip44 account";
}

std::int32_t CmdAllocateBitcoinAddress::runWithOptions()
{
    return run(
        getOption("mynym"),
        getOption("account"),
        getOption("label"),
        getOption("chain"));
}

std::int32_t CmdAllocateBitcoinAddress::run(
    std::string mynym,
    const std::string& account,
    const std::string& label,
    const std::string& chain)
{
    if (false == checkNym("mynym", mynym)) { return -1; }

    OTIdentifier accountID = Identifier::Factory(account);

    if (accountID->empty()) {
        otErr << OT_METHOD << __FUNCTION__ << ": Invalid account ID."
              << std::endl;

        return -1;
    }

    BIP44Chain change{EXTERNAL_CHAIN};

    if ("internal" == chain) { change = INTERNAL_CHAIN; }

    const auto address = Opentxs::Client().Blockchain().AllocateAddress(
        Identifier::Factory(mynym), accountID, label, change);

    if (false == bool(address)) {
        otErr << OT_METHOD << __FUNCTION__ << ": Failed to allocate address."
              << std::endl;

        return -1;
    }

    otOut << "Allocated new address at index " << address->index() << ": "
          << address->address() << std::endl
          << " (" << address->label() << ")" << std::endl;

    return 0;
}
}  // namespace opentxs
