// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdNewOffer.hpp"

#include <opentxs/opentxs.hpp>

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <ostream>
#include <string>
#include <vector>

using namespace opentxs;
using namespace std;

CmdNewOffer::CmdNewOffer()
{
    command = "newoffer";
    // FIX more arguments
    args[0] = "--myacct <assetaccount>";
    args[1] = "--hisacct <currencyaccount>";
    args[2] = "--type <ask|bid>";
    args[3] = "--scale <1|10|100|...>";
    args[4] = "--mininc <min increment>";
    args[5] = "--quantity <quantity>";
    args[6] = "--price <price>";
    args[7] = "[--lifespan <seconds> (default 86400 (1 day))]";
    category = catMarkets;
    help = "Create a new market offer.";
    usage = "A price of 0 means a market order at any price.";
}

CmdNewOffer::~CmdNewOffer() {}

int32_t CmdNewOffer::runWithOptions()
{
    return run(
        getOption("myacct"),
        getOption("hisacct"),
        getOption("type"),
        getOption("scale"),
        getOption("mininc"),
        getOption("quantity"),
        getOption("price"),
        getOption("lifespan"));
}

int32_t CmdNewOffer::run(
    string myacct,
    string hisacct,
    string type,
    string scale,
    string mininc,
    string quantity,
    string price,
    string lifespan)
{
    if (!checkAccount("myacct", myacct)) { return -1; }

    if (!checkAccount("hisacct", hisacct)) { return -1; }

    if (type != "ask" && type != "bid") {
        otOut << "Error: type: mandatory ask/bid parameter not specified.\n";
        return -1;
    }

    if (!checkValue("scale", scale)) { return -1; }

    if (!checkValue("quantity", quantity)) { return -1; }

    if (!checkValue("price", price)) { return -1; }

    if ("" != lifespan && !checkValue("lifespan", lifespan)) { return -1; }

    string server = SwigWrap::GetAccountWallet_NotaryID(myacct);
    if ("" == server) {
        otOut << "Error: cannot determine server from myacct.\n";
        return -1;
    }

    string mynym = SwigWrap::GetAccountWallet_NymID(myacct);
    if ("" == mynym) {
        otOut << "Error: cannot determine mynym from myacct.\n";
        return -1;
    }

    string hisserver = SwigWrap::GetAccountWallet_NotaryID(hisacct);
    if ("" == hisserver) {
        otOut << "Error: cannot determine server from myacct.\n";
        return -1;
    }

    string hisnym = SwigWrap::GetAccountWallet_NymID(hisacct);
    if ("" == hisnym) {
        otOut << "Error: cannot determine hisnym from hisacct.\n";
        return -1;
    }

    if (mynym != hisnym) {
        otOut << "Error: you must own both myacct and hisacct.\n";
        return -1;
    }

    if (server != hisserver) {
        otOut << "Error: accounts must be on the same server.\n";
        return -1;
    }

    // NOTE: Removing this for now. It was a special feature for
    // knotwork and currently it's causing me some problems.
    //
    //    Opentxs::Client().OTME().get_nym_market_offers(server, mynym);
    //
    //    if (0 > cleanMarketOfferList(server, mynym, myacct, hisacct, type,
    //    scale,
    //                                 price)) {
    //        return -1;
    //    }

    // OKAY! Now that we've cleaned out any undesirable offers, let's place the
    // the offer itself!
    int64_t s, m, q, p, l;
    sscanf(scale.c_str(), "%" SCNd64, &s);
    sscanf(mininc.c_str(), "%" SCNd64, &m);
    sscanf(quantity.c_str(), "%" SCNd64, &q);
    sscanf(price.c_str(), "%" SCNd64, &p);
    sscanf(lifespan.c_str(), "%" SCNd64, &l);
    std::string response;
    {
        response = Opentxs::
                       Client()

                       .ServerAction()
                       .CreateMarketOffer(
                           Identifier::Factory(myacct),
                           Identifier::Factory(hisacct),
                           Amount(s),
                           Amount(m),
                           Amount(q),
                           Amount(p),
                           type == "ask",
                           std::chrono::seconds(l),
                           "",
                           Amount(0))
                       ->Run();
    }
    return responseReply(
        response, server, mynym, myacct, "create_market_offer");
}

// NOTE: This function has nothing to do with placing a new offer. Instead,
// as a convenience for knotwork, it first checks to see if there are any
// existing offers within certain parameters based on this new one, and
// removes them if so. Only then, after that is done, does it actually place
// the new offer. (Meaning: most of the code you see here at first is not
// actually necessary for placing offers, but was done at the request of a
// server operator.)

// me: How about this... when you do "opentxs newoffer" I can alter that script
// to automatically cancel any sell offers for a lower amount than my new buy
// offer, if they're on the same market at the same scale. And vice versa.
// Vice versa meaning, cancel any bid offers for a higher amount than my new
// sell offer.
//
// knotwork: yeah that would work.
//
// So when placing a buy offer, check all the other offers I already have at
// the same scale, same asset and currency ID. (That is, the same "market" as
// denoted by strMapKey in "opentxs showmyoffers") For each, see if it's a sell
// offer and if so, if the amount is lower than the amount on the new buy offer,
// then cancel that sell offer from the market. (Because I don't want to
// buy-high, sell-low.)
//
// Similarly, if placing a sell offer, then check all the other offers I already
// have at the same scale, same asset and currency ID, (the same "market" as
// denoted by strMapKey....) For each, see if it's a buy offer and if so, if
// the amount is higher than the amount of my new sell offer, then cancel that
// buy offer from the market. (Because I don't want some old buy offer for $10
// laying around for the same stock that I'm SELLING for $8! If I dump 100
// shares, I'll receive $800 --I don't want my software to automatically turn
// around and BUY those same shares again for $1000! That would be a $200 loss.)

int32_t CmdNewOffer::cleanMarketOfferList(
    const string& server,
    const string& mynym,
    const string& myacct,
    const string& hisacct,
    const string& type,
    const string& scale,
    const string& price)
{
    OTDB::OfferListNym* offerList = loadNymOffers(server, mynym);
    if (nullptr == offerList) {
        otOut << "Error: cannot load market offer list.\n";
        return -1;
    }

    // LOOP THROUGH THE OFFERS and sort them into a map_of_maps, key is:
    // scale-instrumentDefinitionID-currencyID. the value for each key is a
    // sub-map, with
    // the key: transaction ID and value: the offer data itself.
    int32_t items = offerList->GetOfferDataNymCount();
    if (0 > items) {
        otOut << "Error: cannot load market offer list count.\n";
        return -1;
    }

    if (0 == items) {
        otOut << "The market offer list is empty.\n";
        return 0;
    }

    MapOfMaps* map_of_maps = convert_offerlist_to_maps(*offerList);
    if (nullptr == map_of_maps) {
        otOut << "Error: cannot convert offer list to map.\n";
        return -1;
    }
    // (FT) TODO: Fix this ridiculous memory leak. map_of_maps is not
    // cleaned up below this point. (Nor are its member pointers and their
    // contents. unique_ptr is not enough.) I think the only reason Eric
    // let this go was because the program ends anyway after the command
    // fires. Still, needs cleanup.

    // find_strange_offers is called for each offer, for this nym, as it
    // iterates through the maps. When it's done, extra.the_vector
    // will contain a vector of all the transaction numbers for offers that
    // we should cancel, before placing the new offer. (Such as an offer to
    // sell for 30 clams when our new offer buys for 40...)

    the_lambda_struct extra;
    extra.the_asset_acct = myacct;
    extra.the_currency_acct = hisacct;
    extra.the_scale = scale;
    extra.the_price = price;
    extra.bSelling = type == "ask";

    if (0 > iterate_nymoffers_maps(*map_of_maps, find_strange_offers, extra)) {
        otOut << "Error: cannot iterate nym's offers.\n";
        return -1;
    }

    // Okay -- if there are any offers we need to cancel, extra.the_vector
    // now contains the transaction number for each one. Let's remove them from
    // the market before starting up the new offer...
    for (size_t i = 0; i < extra.the_vector.size(); i++) {
        const string& id = extra.the_vector[i];
        otOut << "Canceling market offer with transaction number: " << id
              << ".\n";

        int64_t j;
        sscanf(id.c_str(), "%" SCNd64, &j);
        std::string response;
        {
            response = Opentxs::
                           Client()

                           .ServerAction()
                           .KillMarketOffer(
                               Identifier::Factory(mynym),
                               Identifier::Factory(server),
                               Identifier::Factory(myacct),
                               j)
                           ->Run();
        }
        if (0 > processTxResponse(
                    server, mynym, myacct, response, "kill market offer")) {
            return -1;
        }
    }
    extra.the_vector.clear();

    return 1;
}
