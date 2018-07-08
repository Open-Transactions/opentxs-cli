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

#include <opentxs/opentxs.hpp>

#include "opentxs.hpp"

opentxs::Opentxs::PasswordCallback* callback_{nullptr};
opentxs::OTCaller* password_caller_{nullptr};

int main(int argc, char* argv[])
{
    callback_ = new opentxs::Opentxs::PasswordCallback;

    OT_ASSERT(nullptr != callback_)

    password_caller_ = new opentxs::OTCaller;

    OT_ASSERT(nullptr != password_caller_)

    password_caller_->setCallback(callback_);

    OT_ASSERT(password_caller_->isCallbackSet())

    opentxs::OT::ClientFactory({}, {}, password_caller_);

    int returnValue{0};
    {
        opentxs::Opentxs opentxsCLI;
        returnValue = opentxsCLI.run(argc, argv);
    }

    opentxs::OT::Cleanup();
    delete password_caller_;
    delete callback_;

    return returnValue;
}
