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

#include "CmdRequestOutBailment.hpp"

#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/api/OT.hpp>
#include <opentxs/client/OT_ME.hpp>
#include <opentxs/client/SwigWrap.hpp>

namespace opentxs {

CmdRequestOutBailment::CmdRequestOutBailment()
{
    command = "requestoutbailment";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--hisnym <nym>";
    args[3] = "--mypurse <purse>";
    args[4] = "--amount <amount>";
    category = catOtherUsers;
    help = "Ask the issuer of a unit to process a withdrawal";
}

CmdRequestOutBailment::~CmdRequestOutBailment()
{
}

std::int32_t CmdRequestOutBailment::runWithOptions()
{
    return run(
        getOption("server"),
        getOption("mynym"),
        getOption("hisnym"),
        getOption("mypurse"),
        getOption("amount"));
}

std::int32_t CmdRequestOutBailment::run(
    std::string server,
    std::string mynym,
    std::string hisnym,
    std::string mypurse,
    std::string amount)
{
    if (!checkServer("server", server)) {
        return -1;
    }

    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    if (!checkNym("hisnym", hisnym)) {
        return -1;
    }

    if (!checkPurse("mypurse", mypurse)) {
        return -1;
    }

    std::string terms = inputText("Withdrawal instructions");
    if (0 == terms.size()) {
        return -1;
    }


    std::int64_t outbailmentAmount = SwigWrap::StringToAmount(mypurse, amount);
    if (OT_ERROR_AMOUNT == outbailmentAmount) {
        return -1;
    }


    std::string response = OT::App().API().OTME().initiate_outbailment(
        server, mynym, hisnym, mypurse, outbailmentAmount, terms);
    return processResponse(response, "request outbailment");
}
} // namespace opentxs
