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

#include "CmdDhtQueryServerContract.hpp"

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

CmdDhtQueryServerContract::CmdDhtQueryServerContract()
{
    command = "dhtqueryservercontract";
    args[0] = "--contract <ID of contract to retrieve>";
    category = catAdmin;
    help = "Query a server contract stored in DHT";
    Init();
}

CmdDhtQueryServerContract::~CmdDhtQueryServerContract()
{
}

int32_t CmdDhtQueryServerContract::runWithOptions()
{
    return run(getOption("contract"));
}

int32_t CmdDhtQueryServerContract::run(string contractStr)
{

    DhtResultsCallback gcb(
        [this, contractStr](const DhtResults& values) -> bool {
            return processRetrieved(contractStr, values);
        });

    node_->Retrieve(contractStr, gcb);
    std::this_thread::sleep_for(10s);
    return 1;
}

void CmdDhtQueryServerContract::Init()
{
    DhtConfig config;
    config.enable_dht_ = true;
    node_ = &OpenDHT::It(config);
}

bool CmdDhtQueryServerContract::processRetrieved(
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

        auto contract = proto::DataToProto<proto::ServerContract>(
            Data(data.c_str(), data.size()));

        if (key != contract.id()) {
            continue;
        }

        auto instantiatedContract = OT::App().Wallet().Server(contract);

        if (instantiatedContract) {
            std::string strNotaryID;

            std::string strHostname;
            uint32_t nPort = 0;

            if (!instantiatedContract->ConnectInfo(strHostname, nPort)) {
                otOut << __FUNCTION__
                      << ": Unable to retrieve connection info from "
                         "this contract. Please fix that first; see "
                         "the sample data. (Failure.)\n";
                OT_FAIL;
            }
            strNotaryID = String(instantiatedContract->ID()).Get();
            std::cout << strNotaryID << std::endl;
            std::cout << proto::ProtoAsArmored(
                          instantiatedContract->PublicContract(),
                          "Server Contract") << std::endl;

        } else {
            std::cout << "could not decode Server Contract" << std::endl;
        }

    }
    return true;
}
