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

#ifndef OPENTXS_CLIENT_CMDACKNOWLEDGECONNECTION_HPP
#define OPENTXS_CLIENT_CMDACKNOWLEDGECONNECTION_HPP

#include "CmdBase.hpp"

#include <cstdint>
#include <string>

namespace opentxs
{

class CmdAcknowledgeConnection : public CmdBase
{
public:
    EXPORT CmdAcknowledgeConnection();

    EXPORT std::int32_t run(
        std::string server,
        std::string mynym,
        std::string hisnym,
        std::string mypurse);

    virtual ~CmdAcknowledgeConnection() = default;
protected:
    std::int32_t runWithOptions() override;
};
} // namespace opentxs

#endif // OPENTXS_CLIENT_CMDACKNOWLEDGECONNECTION_HPP
