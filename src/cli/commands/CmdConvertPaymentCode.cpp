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

#include "CmdConvertPaymentCode.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>

namespace opentxs
{

CmdConvertPaymentCode::CmdConvertPaymentCode()
{
    command = "convertpaymentcode";
    args[0] = "--code <payment code>";
    category = catMisc;
    help = "Convert a BIP-47 payment code to a nym ID";
}

std::int32_t CmdConvertPaymentCode::runWithOptions()
{
    return run(getOption("code"));
}

std::int32_t CmdConvertPaymentCode::run(const std::string& code)
{
    if (!checkMandatory("code", code)) { return -1; }

    const std::string id = SwigWrap::NymIDFromPaymentCode(code);

    if ("" == id) {
        otOut << "Error: invalid payment code." << std::endl;

        return -1;
    }

    otOut << id << std::endl;

    return 1;
}
}  // namespace opentxs
