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

#include "CmdNotifyBailment.hpp"

#include <opentxs/opentxs.hpp>

namespace opentxs
{

CmdNotifyBailment::CmdNotifyBailment()
{
    command = "notifybailment";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--hisnym <nym>";
    args[3] = "--mypurse <unit definition id>";
    args[4] = "--request <request ID>";
    args[5] = "--amount <amount>";
    category = catOtherUsers;
    help = "Notify a nym of a pending blockchain deposit";
}

CmdNotifyBailment::~CmdNotifyBailment() {}

std::int32_t CmdNotifyBailment::runWithOptions()
{
    return run(
        getOption("server"),
        getOption("mynym"),
        getOption("hisnym"),
        getOption("mypurse"),
        getOption("request"),
        getOption("amount"));
}

std::int32_t CmdNotifyBailment::run(
    std::string server,
    std::string mynym,
    std::string hisnym,
    std::string mypurse,
    std::string request,
    std::string amount)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    if (!checkNym("hisnym", hisnym)) { return -1; }

    if (!checkPurse("mypurse", mypurse)) { return -1; }

    if (request.empty()) { return -1; }

    std::int64_t notifybailmentAmount =
        SwigWrap::StringToAmount(mypurse, amount);
    if (OT_ERROR_AMOUNT == notifybailmentAmount) { return -1; }

    std::string txid = inputText("Blockchain transaction ID");

    if (0 == txid.size()) { return -1; }

    std::string response;
    {
        response = OT::App()
                       .API()
                       .ServerAction()
                       .NotifyBailment(
                           Identifier::Factory(mynym),
                           Identifier::Factory(server),
                           Identifier::Factory(hisnym),
                           Identifier::Factory(mypurse),
                           Identifier::Factory(request),
                           txid,
                           notifybailmentAmount)
                       ->Run();
    }
    return processResponse(response, "notify bailment");
}
}  // namespace opentxs
