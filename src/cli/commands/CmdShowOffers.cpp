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

#include "CmdShowOffers.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdShowOffers::CmdShowOffers()
{
    command = "showoffers";
    args[0] = "--server <server>";
    args[1] = "--market <marketid>";
    category = catMarkets;
    help = "Show all offers on a particular server and market.";
}

CmdShowOffers::~CmdShowOffers() {}

int32_t CmdShowOffers::runWithOptions()
{
    return run(getOption("server"), getOption("market"));
}

int32_t CmdShowOffers::run(string server, string market)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkMandatory("market", market)) { return -1; }

    OTDB::OfferListMarket* offerList = loadMarketOffers(server, market);
    if (nullptr == offerList) {
        otOut << "Error: cannot load market offer list.\n";
        return -1;
    }

    int32_t bidItems = offerList->GetBidDataCount();
    int32_t askItems = offerList->GetAskDataCount();
    if (0 == bidItems + askItems) {
        cout << "The market offer list is empty.\n";
        return 0;
    }

    // loop through the bids and print them out.
    if (0 != bidItems) {
        cout << "\n** BIDS **\n\nIndex\tTrans#\tPrice\tAvailable\n";

        for (int32_t i = 0; i < bidItems; i++) {
            OTDB::BidData* bid = offerList->GetBidData(i);
            if (nullptr == bid) {
                otOut << "Error: cannot load bid data at index: " << i << "\n";
                return -1;
            }

            cout << i << "\t" << bid->transaction_id << "\t"
                 << bid->price_per_scale << "\t" << bid->available_assets
                 << "\n";
        }
    }

    // loop through the asks and print them out.
    if (0 != askItems) {
        cout << "\n** ASKS **\n\nIndex\tTrans#\tPrice\tAvailable\n";

        for (int32_t i = 0; i < askItems; i++) {
            OTDB::AskData* ask = offerList->GetAskData(i);
            if (nullptr == ask) {
                otOut << "Error: cannot load ask data at index: " << i << "\n";
                return -1;
            }

            cout << i << "\t" << ask->transaction_id << "\t"
                 << ask->price_per_scale << "\t" << ask->available_assets
                 << "\n";
        }
    }

    return 1;
}

OTDB::OfferListMarket* CmdShowOffers::loadMarketOffers(
    const string& server,
    const string& market)
{
    if (!OTDB::Exists("markets", server, "offers", market + ".bin")) {
        return nullptr;
    }

    otWarn << "Offers file exists... Querying file for market offers...\n";

    OTDB::Storable* storable = OTDB::QueryObject(
        OTDB::STORED_OBJ_OFFER_LIST_MARKET,
        "markets",
        server,
        "offers",
        market + ".bin");
    if (nullptr == storable) {
        otOut << "Unable to verify storable object. Probably doesn't "
                 "exist.\n";
        return nullptr;
    }

    otWarn << "QueryObject worked. Now dynamic casting from storable to a "
              "(market) offerList...\n";

    OTDB::OfferListMarket* offerList =
        dynamic_cast<OTDB::OfferListMarket*>(storable);
    if (nullptr == offerList) {
        otOut << "Unable to dynamic cast a storable to a (market) "
                 "offerList.\n";
        return nullptr;
    }

    return offerList;
}
