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

#include "CmdArchivePeerReply.hpp"

#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/client/OTAPI_Exec.hpp>
#include <opentxs/OT.hpp>

namespace opentxs {

CmdArchivePeerReply::CmdArchivePeerReply()
{
    command = "archivepeerreply";
    args[0] = "--mynym <nym>";
    args[1] = "--mypurse <reply ID>";
    category = catOtherUsers;
    help = "Move a reply to the processed box";
}

std::int32_t CmdArchivePeerReply::runWithOptions()
{
    return run(getOption("mynym"), getOption("mypurse"));
}

std::int32_t CmdArchivePeerReply::run(std::string mynym, std::string mypurse)
{
    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    return OT::App().API().Exec().completePeerRequest(mynym, mypurse);
}
} // namespace opentxs
