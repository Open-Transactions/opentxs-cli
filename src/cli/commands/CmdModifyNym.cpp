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

#include "CmdModifyNym.hpp"

#include "CmdBase.hpp"

#include <opentxs/core/Version.hpp>
#include <opentxs/client/SwigWrap.hpp>
#include <opentxs/core/Log.hpp>

namespace opentxs
{
CmdModifyNym::CmdModifyNym()
{
    command = "modifynym";
    args[0] = "--mynym <nym>";
    args[1] = "--individual <label>";
    args[2] = "--server <label>";
    args[3] = "--opentxs <server id>";
    args[4] = "--core <payment code>";
    args[5] = "--testcore <payment code>";
    args[6] = "--cash <payment code>";
    args[7] = "--testcash <payment code>";
    category = catWallet;
    help = "Edit a nym's contact credential data.";
}

std::int32_t CmdModifyNym::runWithOptions()
{
    return run(
        getOption("mynym"),
        getOption("individual"),
        getOption("server"),
        getOption("opentxs"),
        getOption("core"),
        getOption("cash"),
        getOption("testcore"),
        getOption("testcash"));
}

std::int32_t CmdModifyNym::run(
    std::string mynym,
    const std::string& individual,
    const std::string& server,
    const std::string& opentxs,
    const std::string& core,
    const std::string& cash,
    const std::string& testCore,
    const std::string& testCash)
{
    if (false == checkNym("mynym", mynym)) {
        otErr << "Unknown nym " << mynym << std::endl;

        return -1;
    }

    auto data = SwigWrap::Wallet_GetNym(mynym);

    if (false == data.Valid()) {
        otErr << "Invalid nym " << mynym << std::endl;

        return -1;
    }

    if (false == individual.empty()) {
        data.SetType(proto::CITEMTYPE_INDIVIDUAL, individual);
    }

    if (false == server.empty()) {
        data.SetType(proto::CITEMTYPE_SERVER, server);
    }

    if (false == opentxs.empty()) {
        data.AddPreferredOTServer(opentxs, true);
    }

    if (false == core.empty()) {
        data.AddPaymentCode(core, proto::CITEMTYPE_BTC, true, true);
    }

    if (false == testCore.empty()) {
        data.AddPaymentCode(testCore, proto::CITEMTYPE_TNBTC, true, true);
    }

    if (false == cash.empty()) {
        data.AddPaymentCode(cash, proto::CITEMTYPE_BCH, true, true);
    }

    if (false == testCash.empty()) {
        data.AddPaymentCode(testCash, proto::CITEMTYPE_TNBCH, true, true);
    }

    otErr << SwigWrap::DumpContactData(mynym) << std::endl;

    return 1;
}
} // namespace opentxs
