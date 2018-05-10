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

#ifndef OPENTXS_CLIENT_CMDSHOWCONTACT_HPP
#define OPENTXS_CLIENT_CMDSHOWCONTACT_HPP

#include "CmdBase.hpp"

#include <cstdint>
#include <string>

namespace opentxs
{

class CmdShowContact : public CmdBase
{
public:
    CmdShowContact();

    std::int32_t run(const std::string& id);

    ~CmdShowContact() = default;

private:
    void display_groups(const ui::ContactSection& section) const;
    void display_items(const ui::ContactSubsection& group) const;
    std::int32_t runWithOptions() override;
};
} // namespace opentxs

#endif // OPENTXS_CLIENT_CMDSHOWCONTACT_HPP
