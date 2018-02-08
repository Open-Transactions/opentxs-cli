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

#include "CmdDhtStoreNym.hpp"

#include "CmdBase.hpp"

#include <opentxs/api/client/Wallet.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/api/network/Dht.hpp>
#include <opentxs/OT.hpp>
#include <opentxs/client/SwigWrap.hpp>
#include <opentxs/core/Identifier.hpp>
#include <opentxs/Proto.hpp>

#include <iostream>
#include <thread>

using namespace opentxs;
using namespace std;

CmdDhtStoreNym::CmdDhtStoreNym()
{
    command = "dhtstorenym";
    args[0] = "--nym <ID of nym to store>";
    category = catAdmin;
    help = "Store a nym into DHT";
}

CmdDhtStoreNym::~CmdDhtStoreNym()
{
}

int32_t CmdDhtStoreNym::runWithOptions()
{
    return run(getOption("nym"));
}

int32_t CmdDhtStoreNym::run(string nymStr)
{

    auto nym = OT::App().Wallet().Nym(Identifier(nymStr));

    if (!nym) {
        exit (1);
    }

    otOut << "Storing nym: \n" << nymStr << "\n";
    OT::App().DHT().Insert(nym->asPublicNym());

    std::this_thread::sleep_for(10s);

    return 1;
}
