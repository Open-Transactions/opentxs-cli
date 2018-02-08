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

#include "CmdDhtQueryNym.hpp"

#include "CmdBase.hpp"

#include <opentxs/api/client/Wallet.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/api/network/Dht.hpp>
#include "opentxs/network/DhtConfig.hpp"
#include "opentxs/network/OpenDHT.hpp"
#include <opentxs/OT.hpp>
#include <opentxs/client/SwigWrap.hpp>
#include <opentxs/core/Identifier.hpp>
#include <opentxs/Proto.hpp>

#include <iostream>
#include <thread>

using namespace opentxs;
using namespace std;

CmdDhtQueryNym::CmdDhtQueryNym()
{
    command = "dhtquerynym";
    args[0] = "--nym <ID of nym to retrieve>";
    category = catAdmin;
    help = "Query a nym stored in DHT";
    Init();
}

CmdDhtQueryNym::~CmdDhtQueryNym()
{
}

int32_t CmdDhtQueryNym::runWithOptions()
{
    return run(getOption("nym"));
}

int32_t CmdDhtQueryNym::run(string nymStr)
{

    DhtResultsCallback gcb(
        [this, nymStr](const DhtResults& values) -> bool {
            return processRetrieved(nymStr, values);
        });

    node_->Retrieve(nymStr, gcb);
    std::this_thread::sleep_for(10s);
    return 1;
}

void CmdDhtQueryNym::Init()
{
    DhtConfig config;
    config.enable_dht_ = true;
    node_ = &OpenDHT::It(config);
}

bool CmdDhtQueryNym::processRetrieved(
    const std::string key,
    const DhtResults& values)
{
    if (key.empty()) {
        return false;
    }

    for (const auto& it : values) {
        if (nullptr == it) {
            continue;
        }

        auto& data = *it;

        if (0 == data.size()) {
            continue;
        }

        auto publicNym = proto::DataToProto<proto::CredentialIndex>(
            Data(data.c_str(), data.size()));

        auto instantiatedNym = OT::App().Wallet().Nym(publicNym);

        if (instantiatedNym) {
            const auto armored =
                proto::ProtoAsArmored(instantiatedNym->asPublicNym(), "PUBLIC NYM");
            std::cout << armored << std::endl;


        } else {
            std::cout << "could not decode publicNym" << std::endl;
        }

    }
    return true;
}
