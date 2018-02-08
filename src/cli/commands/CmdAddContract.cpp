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

#include "CmdAddContract.hpp"

#include "CmdBase.hpp"

#include <opentxs/client/SwigWrap.hpp>
#include <opentxs/core/Log.hpp>

namespace opentxs
{
CmdAddContract::CmdAddContract()
{
    command = "addcontract";
    args[0] = "--mynym <nym>";
    args[1] = "--type <currency>";
    args[2] = "--value <instrument definition id>";
    category = catWallet;
    help = "Edit a nym's contact credential data.";
}

std::int32_t CmdAddContract::runWithOptions()
{
    return run(
        getOption("mynym"),
        getOption("type"),
        getOption("value"));
}

std::int32_t CmdAddContract::run(
    std::string mynym,
    const std::string& type,
    const std::string& value)
{
    if (false == checkNym("mynym", mynym)) {
        otErr << "Unknown nym " << mynym << std::endl;

        return -1;
    }

    if (type.empty()) {
        otErr << "Invalid type" << std::endl;

        return -1;
    }

    if (value.empty()) {
        otErr << "Invalid value" << std::endl;

        return -1;
    }

    const auto currency = static_cast<proto::ContactItemType>(
        std::stoi(type));
    auto data = SwigWrap::Wallet_GetNym(mynym);

    if (false == data.Valid()) {
        otErr << "Invalid nym " << mynym << std::endl;

        return -1;
    }

    const auto set = data.AddContract(
        value,
        currency,
        true,
        true);

    if (false == set) {
        otErr << "Failed to set claim" << std::endl;

        return -1;
    }

    otErr << SwigWrap::DumpContactData(mynym) << std::endl;

    return 1;
}
} // namespace opentxs
