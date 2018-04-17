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

#ifndef OPENTXS_CLIENT_CMDSENDMESSAGE_HPP
#define OPENTXS_CLIENT_CMDSENDMESSAGE_HPP

#include "CmdBase.hpp"

#include <cstdint>
#include <string>

namespace opentxs
{

class CmdSendMessage : public CmdBase
{
public:
    EXPORT CmdSendMessage();
    EXPORT virtual ~CmdSendMessage() = default;

    EXPORT std::int32_t run(
        std::string server,
        std::string mynym,
        std::string hisnym);

protected:
    std::int32_t runWithOptions() override;

private:
    std::int32_t nym(
        const std::string& server,
        const std::string& mynym,
        std::string& hisnym,
        const std::string& message);

    std::int32_t contact(
        const std::string& mynym,
        const std::string& hisnym,
        const std::string& message);
};
} // namespace opentxs

#endif // OPENTXS_CLIENT_CMDSENDMESSAGE_HPP
