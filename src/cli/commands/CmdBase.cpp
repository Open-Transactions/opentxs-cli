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

#include "CmdBase.hpp"

#include <opentxs/api/client/ServerAction.hpp>
#include <opentxs/api/client/Wallet.hpp>
#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/client/OT_API.hpp>
#include <opentxs/client/OTAPI_Exec.hpp>
#include <opentxs/client/OTWallet.hpp>
#include <opentxs/client/ServerAction.hpp>
#include <opentxs/client/SwigWrap.hpp>
#include <opentxs/client/Utility.hpp>
#include <opentxs/core/contract/ServerContract.hpp>
#include <opentxs/core/contract/UnitDefinition.hpp>
#include <opentxs/core/util/Assert.hpp>
#include <opentxs/core/util/Common.hpp>
#include <opentxs/core/Account.hpp>
#include <opentxs/core/Identifier.hpp>
#include <opentxs/core/Log.hpp>
#include <opentxs/core/Nym.hpp>
#include <opentxs/core/String.hpp>
#include <opentxs/ext/Helpers.hpp>
#include <opentxs/OT.hpp>

#include <ctype.h>
#include <stdint.h>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace opentxs;
using namespace std;

// used for passing and returning values when giving a
// lambda function to a loop function.
//
// cppcheck-suppress uninitMemberVar
the_lambda_struct::the_lambda_struct() {}

MapOfMaps* convert_offerlist_to_maps(OTDB::OfferListNym& offerList)
{
    std::string strLocation = "convert_offerlist_to_maps";

    MapOfMaps* map_of_maps = nullptr;

    // LOOP THROUGH THE OFFERS and sort them std::into a map_of_maps, key is:
    // scale-instrumentDefinitionID-currencyID
    // the value for each key is a sub-map, with the key: transaction ID and
    // value: the offer data itself.
    //
    std::int32_t nCount = offerList.GetOfferDataNymCount();
    std::int32_t nTemp = nCount;

    if (nCount > 0) {
        for (std::int32_t nIndex = 0; nIndex < nCount; ++nIndex) {

            nTemp = nIndex;
            OTDB::OfferDataNym* offerDataPtr = offerList.GetOfferDataNym(nTemp);

            if (!offerDataPtr) {
                otOut << strLocation
                      << ": Unable to reference (nym) offerData "
                         "on offerList, at index: "
                      << nIndex << "\n";
                return map_of_maps;
            }

            OTDB::OfferDataNym& offerData = *offerDataPtr;
            std::string strScale = offerData.scale;
            std::string strInstrumentDefinitionID =
                offerData.instrument_definition_id;
            std::string strCurrencyTypeID = offerData.currency_type_id;
            std::string strSellStatus = offerData.selling ? "SELL" : "BUY";
            std::string strTransactionID = offerData.transaction_id;

            std::string strMapKey = strScale + "-" + strInstrumentDefinitionID +
                                    "-" + strCurrencyTypeID;

            SubMap* sub_map = nullptr;
            if (nullptr != map_of_maps && !map_of_maps->empty() &&
                (map_of_maps->count(strMapKey) > 0)) {
                sub_map = (*map_of_maps)[strMapKey];
            }

            if (nullptr != sub_map) {
                otWarn << strLocation << ": The sub-map already exists!\n";

                // Let's just add this offer to the existing submap
                // (There must be other offers already there for the same
                // market, since the submap already exists.)
                //
                // the sub_map for this market is mapped by BUY/SELL ==> the
                // actual offerData.
                //

                (*sub_map)[strTransactionID] = &offerData;
            } else  // submap does NOT already exist for this market. (Create
                    // it...)
            {
                otWarn << strLocation
                       << ": The sub-map does NOT already exist!\n";
                //
                // Let's create the submap with this new offer, and add it
                // to the main map.
                //
                sub_map = new SubMap;
                (*sub_map)[strTransactionID] = &offerData;

                if (nullptr == map_of_maps) {
                    map_of_maps = new MapOfMaps;
                }

                (*map_of_maps)[strMapKey] = sub_map;
            }

            // Supposedly by this point I have constructed a map keyed by the
            // market, which returns a sub_map for each market. Each sub map
            // uses the key "BUY" or "SELL" and that points to the actual
            // offer data. (Like a Multimap.)
            //
            // Therefore we have sorted out all the buys and sells for each
            // market. Later on, we can loop through the main map, and for each
            // market, we can loop through all the buys and sells.
        }  // for (constructing the map_of_maps and all the sub_maps, so that
           // the
           // offers are sorted
           // by market and buy/sell status.
    }

    return map_of_maps;
}

// If you have a buy offer, to buy silver for $30, and to sell silver for $35,
// what happens tomorrow when the market shifts, and you want to buy for $40
// and sell for $45 ?
//
// Well, now you need to cancel certain sell orders from yesterday! Because why
// on earth would you want to sell silver for $35 while buying it for $40?
// (knotwork raised ) That would be buy-high, sell-low.
//
// Any rational trader would cancel the old $35 sell order before placing a new
// $40 buy order!
//
// Similarly, if the market went DOWN such that my old offers were $40 buy / $45
// sell, and my new offers are going to be $30 buy / $35 sell, then I want to
// cancel certain buy orders for yesterday. After all, why on earth would you
// want to buy silver for $40 meanwhile putting up a new sell order at $35!
// You would immediately just turn around, after buying something, and sell it
// for LESS?
//
// Since the user would most likely be forced anyway to do this, for reasons of
// self-interest, it will probably end up as the default behavior here.
//

// RETURN VALUE: extra_vals will contain a list of offers that need to be
// removed AFTER

std::int32_t find_strange_offers(
    const OTDB::OfferDataNym& offer_data,
    const std::int32_t,
    const MapOfMaps&,
    const SubMap&,
    the_lambda_struct& extra_vals)  // if 10 offers are
                                    // printed
                                    // for the SAME market,
                                    // nIndex will be 0..9
{
    std::string strLocation = "find_strange_offers";
    /*
    me: How about this — when you do "opentxs newoffer" I can alter that
    script to automatically cancel any sell offers for a lower amount
    than my new buy offer, if they're on the same market at the same scale.
    and vice versa. Vice versa meaning, cancel any bid offers for a higher
    amount than my new sell offer.

    knotwork: yeah that would work.

    So when placing a buy offer, check all the other offers I already have at
    the same scale,
    same asset and currency ID. (That is, the same "market" as denoted by
    strMapKey in "opentxs showmyoffers")
    For each, see if it's a sell offer and if so, if the amount is lower than
    the amount on
    the new buy offer, then cancel that sell offer from the market. (Because I
    don't want to buy-high, sell low.)

    Similarly, if placing a sell offer, then check all the other offers I
    already have at the
    same scale, same asset and currency ID, (the same "market" as denoted by
    strMapKey....) For
    each, see if it's a buy offer and if so, if the amount is higher than the
    amount of my new
    sell offer, then cancel that buy offer from the market. (Because I don't
    want some old buy offer
    for $10 laying around for the same stock that I'm SELLING for $8! If I dump
    100 shares, I'll receive
    $800--I don't want my software to automatically turn around and BUY those
    same shares again for $1000!
    That would be a $200 loss.)

    This is done here. This function gets called once for each offer that's
    active for this Nym.
    extra_vals contains the relevant info we're looking for, and offer_data
    contains the current
    offer (as we loop through ALL this Nym's offers, this function gets called
    for each one.)
    So here we just need to compare once, and add to the list if the comparison
    matches.
    */
    /*
    attr the_lambda_struct::the_vector        // used for returning a list of
    something.
    attr the_lambda_struct::the_asset_acct    // for newoffer, we want to remove
    existing offers for the same accounts in certain cases.
    attr the_lambda_struct::the_currency_acct // for newoffer, we want to remove
    existing offers for the same accounts in certain cases.
    attr the_lambda_struct::the_scale         // for newoffer as well.
    attr the_lambda_struct::the_price         // for newoffer as well.
    attr the_lambda_struct::bSelling          // for newoffer as well.
    */
    otLog4 << strLocation
           << ": About to compare the new potential offer "
              "against one of the existing ones...";

    if ((extra_vals.the_asset_acct == offer_data.asset_acct_id) &&
        (extra_vals.the_currency_acct == offer_data.currency_acct_id) &&
        (extra_vals.the_scale == offer_data.scale)) {
        otLog4 << strLocation << ": the account IDs and the scale match...";

        // By this point we know the current offer_data has the same asset acct,
        // currency acct, and scale
        // as the offer we're comparing to all the rest.
        //
        // But that's not enough: we also need to compare some prices:
        //
        // So when placing a buy offer, check all the other offers I already
        // have.
        // For each, see if it's a sell offer and if so, if the amount is lower
        // than the amount on
        // the new buy offer, then cancel that sell offer from the market.
        // (Because I don't want to buy-high, sell low.)
        //
        if (!extra_vals.bSelling && offer_data.selling &&
            (stoll(offer_data.price_per_scale) < stoll(extra_vals.the_price))) {
            extra_vals.the_vector.push_back(offer_data.transaction_id);
        }
        // Similarly, when placing a sell offer, check all the other offers I
        // already have.
        // For each, see if it's a buy offer and if so, if the amount is higher
        // than the amount of my new
        // sell offer, then cancel that buy offer from the market.
        //
        else if (
            extra_vals.bSelling && !offer_data.selling &&
            (stoll(offer_data.price_per_scale) > stoll(extra_vals.the_price))) {
            extra_vals.the_vector.push_back(offer_data.transaction_id);
        }
    }
    // We don't actually do the removing here, since we are still looping
    // through the maps.
    // So we just add the IDs to a vector so that the caller can do the removing
    // once this loop is over.

    return 1;
}

// low level. map_of_maps and sub_map must be good. (assumed.)
//
// extra_vals allows you to pass any extra data you want std::into your
// lambda, for when it is called. (Like a functor.)
//
std::int32_t iterate_nymoffers_sub_map(
    const MapOfMaps& map_of_maps,
    SubMap& sub_map,
    LambdaFunc the_lambda,
    the_lambda_struct& extra_vals)
{
    // the_lambda must be good (assumed) and must have the parameter profile
    // like this sample:
    // def the_lambda(offer_data, nIndex, map_of_maps, sub_map, extra_vals)
    //
    // if 10 offers are printed for the SAME market, nIndex will be 0..9

    std::string strLocation = "iterate_nymoffers_sub_map";

    // Looping through the map_of_maps, we are now on a valid sub_map in this
    // iteration.
    // Therefore let's loop through the offers on that sub_map and output them!
    //
    // var range_sub_map = sub_map.range();

    SubMap* sub_mapPtr = &sub_map;
    if (!sub_mapPtr) {
        otOut << strLocation
              << ": No range retrieved from sub_map. It must be "
                 "non-existent, I guess.\n";
        return -1;
    }
    if (sub_map.empty()) {
        // Should never happen since we already made sure all the sub_maps
        // have data on them. Therefore if this range is empty now, it's a
        // chaiscript
        // bug (extremely unlikely.)
        //
        otOut << strLocation
              << ": Error: A range was retrieved for the "
                 "sub_map, but the range is empty.\n";
        return -1;
    }

    std::int32_t nIndex = -1;
    for (auto it = sub_map.begin(); it != sub_map.end(); ++it) {
        ++nIndex;
        // var offer_data_pair = range_sub_map.front();

        if (nullptr == it->second) {
            otOut << strLocation
                  << ": Looping through range_sub_map range, "
                     "and first offer_data_pair fails to "
                     "verify.\n";
            return -1;
        }

        OTDB::OfferDataNym& offer_data = *it->second;
        std::int32_t nLambda = (*the_lambda)(
            offer_data,
            nIndex,
            map_of_maps,
            sub_map,
            extra_vals);  // if 10 offers are printed for the SAME
                          // market, nIndex will be 0..9;
        if (-1 == nLambda) {
            otOut << strLocation << ": Error: the_lambda failed.\n";
            return -1;
        }
    }
    sub_map.clear();

    return 1;
}

std::int32_t iterate_nymoffers_maps(
    MapOfMaps& map_of_maps,
    LambdaFunc the_lambda)  // low level. map_of_maps
                            // must be
                            // good. (assumed.)
{
    the_lambda_struct extra_vals;
    return iterate_nymoffers_maps(map_of_maps, the_lambda, extra_vals);
}

// extra_vals allows you to pass any extra data you want std::into your
// lambda, for when it is called. (Like a functor.)
//
std::int32_t iterate_nymoffers_maps(
    MapOfMaps& map_of_maps,
    LambdaFunc the_lambda,
    the_lambda_struct& extra_vals)  // low level.
                                    // map_of_maps
                                    // must be good.
                                    // (assumed.)
{
    // the_lambda must be good (assumed) and must have the parameter profile
    // like this sample:
    // def the_lambda(offer_data, nIndex, map_of_maps, sub_map, extra_vals)
    // //
    // if 10 offers are printed for the SAME market, nIndex will be 0..9

    std::string strLocation = "iterate_nymoffers_maps";

    // Next let's loop through the map_of_maps and output the offers for each
    // market therein...
    //
    // var range_map_of_maps = map_of_maps.range();
    MapOfMaps* map_of_mapsPtr = &map_of_maps;
    if (!map_of_mapsPtr) {
        otOut << strLocation << ": No range retrieved from map_of_maps.\n";
        return -1;
    }
    if (map_of_maps.empty()) {
        otOut << strLocation
              << ": A range was retrieved for the map_of_maps, "
                 "but the range is empty.\n";
        return -1;
    }

    for (auto it = map_of_maps.begin(); it != map_of_maps.end(); ++it) {
        // var sub_map_pair = range_map_of_maps.front();
        if (nullptr == it->second) {
            otOut << strLocation
                  << ": Looping through map_of_maps range, and "
                     "first sub_map_pair fails to verify.\n";
            return -1;
        }

        std::string strMapKey = it->first;

        SubMap& sub_map = *it->second;
        if (sub_map.empty()) {
            otOut << strLocation
                  << ": Error: Sub_map is empty (Then how is it "
                     "even here?? Submaps are only added based "
                     "on existing offers.)\n";
            return -1;
        }

        std::int32_t nSubMap = iterate_nymoffers_sub_map(
            map_of_maps, sub_map, the_lambda, extra_vals);
        if (-1 == nSubMap) {
            otOut << strLocation
                  << ": Error: while trying to iterate_nymoffers_sub_map.\n";
            return -1;
        }
    }
    map_of_maps.clear();

    return 1;
}

OTDB::OfferListNym* loadNymOffers(
    const std::string& notaryID,
    const std::string& nymID)
{
    OTDB::OfferListNym* offerList = nullptr;

    if (OTDB::Exists("nyms", notaryID, "offers", nymID + ".bin")) {
        otWarn << "Offers file exists... Querying nyms...\n";
        OTDB::Storable* storable = OTDB::QueryObject(
            OTDB::STORED_OBJ_OFFER_LIST_NYM,
            "nyms",
            notaryID,
            "offers",
            nymID + ".bin");

        if (nullptr == storable) {
            otOut << "Unable to verify storable object. Probably doesn't "
                     "exist.\n";
            return nullptr;
        }

        otWarn << "QueryObject worked. Now dynamic casting from storable to a "
                  "(nym) offerList...\n";
        offerList = dynamic_cast<OTDB::OfferListNym*>(storable);

        if (nullptr == offerList) {
            otOut
                << "Unable to dynamic cast a storable to a (nym) offerList.\n";
            return nullptr;
        }
    }

    return offerList;
}

std::int32_t output_nymoffer_data(
    const OTDB::OfferDataNym& offer_data,
    std::int32_t nIndex,
    const MapOfMaps&,
    const SubMap&,
    the_lambda_struct&)  // if 10 offers are printed for the
                         // SAME market, nIndex will be 0..9
{  // extra_vals unused in this function, but not in others that share this
    // parameter profile.
    // (It's used as a lambda.)

    std::string strScale = offer_data.scale;
    std::string strInstrumentDefinitionID = offer_data.instrument_definition_id;
    std::string strCurrencyTypeID = offer_data.currency_type_id;
    std::string strSellStatus = offer_data.selling ? "SELL" : "BUY";
    std::string strTransactionID = offer_data.transaction_id;
    std::string strAvailableAssets = std::to_string(
        std::stoll(offer_data.total_assets) -
        std::stoll(offer_data.finished_so_far));

    if (0 == nIndex)  // first iteration! (Output a header.)
    {
        otOut << "\nScale:\t\t" << strScale << "\n";
        otOut << "Asset:\t\t" << strInstrumentDefinitionID << "\n";
        otOut << "Currency:\t" << strCurrencyTypeID << "\n";
        otOut << "\nIndex\tTrans#\tType\tPrice\tAvailable\n";
    }

    //
    // Okay, we have the offer_data, so let's output it!
    //
    std::cout << (nIndex) << "\t" << offer_data.transaction_id << "\t"
              << strSellStatus << "\t" << offer_data.price_per_scale << "\t"
              << strAvailableAssets << "\n";

    return 1;
}

CmdBase::CmdBase()
    : category(catError)
    , command(nullptr)
    , help(nullptr)
    , usage(nullptr)
{
    for (int i = 0; i < MAX_ARGS; i++) {
        args[i] = nullptr;
    }
}

CmdBase::~CmdBase() {}

// CHECK USER (download a public key)
//
std::string CmdBase::check_nym(
    const std::string& notaryID,
    const std::string& nymID,
    const std::string& targetNymID) const
{
    auto action = OT::App().API().ServerAction().DownloadNym(
        Identifier(nymID), Identifier(notaryID), Identifier(targetNymID));

    return action->Run();
}

bool CmdBase::checkAccount(const char* name, string& account) const
{
    if (!checkMandatory(name, account)) {
        return false;
    }

    std::shared_ptr<Account> pAccount{nullptr};
    OTWallet* wallet = getWallet();
    Identifier theID(account);

    if (!theID.empty()) pAccount = wallet->GetAccount(theID);

    if (false == bool(pAccount)) {
        pAccount = wallet->GetAccountPartialMatch(account);

        if (false == bool(pAccount)) {
            otOut << "Error: " << name << ": unknown account: " << account
                  << "\n";

            return false;
        }
    }

    if (pAccount) {
        String tmp;
        pAccount->GetPurportedAccountID().GetString(tmp);
        account = tmp.Get();
    }

    otOut << "Using " << name << ": " << account << "\n";

    return true;
}

int64_t CmdBase::checkAmount(
    const char* name,
    const string& amount,
    const string& myacct) const
{
    if (!checkMandatory(name, amount)) {
        return OT_ERROR_AMOUNT;
    }

    string assetType = getAccountAssetType(myacct);
    if ("" == assetType) {
        return OT_ERROR_AMOUNT;
    }

    int64_t value = SwigWrap::StringToAmount(assetType, amount);
    if (OT_ERROR_AMOUNT == value) {
        otOut << "Error: " << name << ": invalid amount: " << amount << "\n";
        return OT_ERROR_AMOUNT;
    }

    return value;
}

bool CmdBase::checkFlag(const char* name, const string& value) const
{
    if (!checkMandatory(name, value)) {
        return false;
    }

    if (value != "false" && value != "true") {
        otOut << "Error: " << name << ": expected 'false' or 'true'.\n";
        return false;
    }

    return true;
}

int32_t CmdBase::checkIndex(
    const char* name,
    const string& index,
    int32_t items) const
{
    if (!checkValue(name, index)) {
        return -1;
    }

    if (!checkIndicesRange(name, index, items)) {
        return -1;
    }

    return stoi(index);
}

bool CmdBase::checkIndices(const char* name, const string& indices) const
{
    if (!checkMandatory(name, indices)) {
        return false;
    }

    if ("all" == indices) {
        return true;
    }

    for (string::size_type i = 0; i < indices.length(); i++) {
        if (!isdigit(indices[i])) {
            otOut << "Error: " << name << ": not a value: " << indices << "\n";
            return false;
        }
        for (i++; i < indices.length() && isdigit(indices[i]); i++) {
        }
        if (i < indices.length() && ',' != indices[i]) {
            otOut << "Error: " << name << ": not a value: " << indices << "\n";
            return false;
        }
    }

    return true;
}

bool CmdBase::checkIndicesRange(
    const char* name,
    const string& indices,
    int32_t items) const
{
    if ("all" == indices) {
        return true;
    }

    for (string::size_type i = 0; i < indices.length(); i++) {
        int32_t value = 0;
        for (; isdigit(indices[i]); i++) {
            value = value * 10 + indices[i] - '0';
        }
        if (0 > value || value >= items) {
            otOut << "Error: " << name << ": value (" << value
                  << ") out of range (must be < " << items << ")\n";
            return false;
        }
    }

    return true;
}

bool CmdBase::checkMandatory(const char* name, const string& value) const
{
    if ("" == value) {
        otOut << "Error: " << name << ": mandatory parameter not specified.\n";
        return false;
    }

    return true;
}

bool CmdBase::checkNym(const char* name, string& nym, bool checkExistance) const
{
    if (!checkMandatory(name, nym)) return false;

    ConstNym pNym = nullptr;
    const Identifier nymID(nym);

    if (!nymID.empty()) pNym = OT::App().Wallet().Nym(nymID);

    if (false == bool(pNym)) pNym = OT::App().Wallet().NymByIDPartialMatch(nym);

    if (nullptr != pNym) {
        String tmp;
        pNym->GetIdentifier(tmp);
        nym = tmp.Get();
    } else if (checkExistance) {
        otOut << "Error: " << name << ": unknown nym: " << nym << "\n";
        return false;
    }

    otOut << "Using " << name << ": " << nym << "\n";
    return true;
}

bool CmdBase::checkPurse(const char* name, string& purse) const
{
    if (!checkMandatory(name, purse)) return false;

    Identifier theID(purse);
    ConstUnitDefinition pUnit;  // shared_ptr to const.

    // See if it's available using the full length ID.
    if (!theID.empty()) pUnit = OT::App().Wallet().UnitDefinition(theID);

    if (!pUnit) {
        const auto units = OT::App().Wallet().UnitDefinitionList();

        // See if it's available using the partial length ID.
        for (auto& it : units) {
            if (0 == it.first.compare(0, purse.length(), purse)) {
                pUnit = OT::App().Wallet().UnitDefinition(Identifier(it.first));
                break;
            }
        }
        if (!pUnit) {
            // See if it's available using the full length name.
            for (auto& it : units) {
                if (0 == it.second.compare(0, it.second.length(), purse)) {
                    pUnit =
                        OT::App().Wallet().UnitDefinition(Identifier(it.first));
                    break;
                }
            }

            if (!pUnit) {
                // See if it's available using the partial name.
                for (auto& it : units) {
                    if (0 == it.second.compare(0, purse.length(), purse)) {
                        pUnit = OT::App().Wallet().UnitDefinition(
                            Identifier(it.first));
                        break;
                    }
                }
            }
        }
    }

    if (!pUnit) {
        otOut << "Error: " << name << ": unknown unit definition: " << purse
              << "\n";
        return false;
    }

    purse = String(pUnit->ID()).Get();
    otOut << "Using " << name << ": " << purse << "\n";
    return true;
}

bool CmdBase::checkServer(const char* name, string& server) const
{
    if (!checkMandatory(name, server)) return false;

    Identifier theID(server);
    ConstServerContract pServer;  // shared_ptr to const.

    // See if it's available using the full length ID.
    if (!theID.empty()) pServer = OT::App().Wallet().Server(theID);

    if (!pServer) {
        const auto servers = OT::App().Wallet().ServerList();

        // See if it's available using the partial length ID.
        for (auto& it : servers) {
            if (0 == it.first.compare(0, server.length(), server)) {
                pServer = OT::App().Wallet().Server(Identifier(it.first));
                break;
            }
        }
        if (!pServer) {
            // See if it's available using the full length name.
            for (auto& it : servers) {
                if (0 == it.second.compare(0, it.second.length(), server)) {
                    pServer = OT::App().Wallet().Server(Identifier(it.first));
                    break;
                }
            }

            if (!pServer) {
                // See if it's available using the partial name.
                for (auto& it : servers) {
                    if (0 == it.second.compare(0, server.length(), server)) {
                        pServer =
                            OT::App().Wallet().Server(Identifier(it.first));
                        break;
                    }
                }
            }
        }
    }

    if (!pServer) {
        otOut << "Error: " << name << ": unknown server: " << server << "\n";
        return false;
    }

    server = String(pServer->ID()).Get();
    otOut << "Using " << name << ": " << server << "\n";
    return true;
}

int64_t CmdBase::checkTransNum(const char* name, const string& id) const
{
    if (!checkMandatory(name, id)) {
        return -1;
    }

    for (string::size_type i = 0; i < id.length(); i++) {
        if (!isdigit(id[i])) {
            otOut << "Error: " << name << ": not a value: " << id << "\n";
            return -1;
        }
    }

    int64_t value = stoll(id);
    if (0 >= value) {
        otOut << "Error: " << name << ": invalid value: " << id << "\n";
        return -1;
    }

    return value;
}

bool CmdBase::checkValue(const char* name, const string& value) const
{
    if (!checkMandatory(name, value)) {
        return false;
    }

    for (string::size_type i = 0; i < value.length(); i++) {
        if (!isdigit(value[i])) {
            otOut << "Error: " << name << ": not a value: " << value << "\n";
            return false;
        }
    }

    return true;
}

void CmdBase::dashLine() const
{
    // 76 dashes :-)
    cout << "--------------------------------------"
            "--------------------------------------\n";
}

const vector<string>& CmdBase::extractArgumentNames()
{
    // only do this once
    if (0 != argNames.size()) {
        return argNames;
    }

    // extract argument names from usage help text
    for (int i = 0; i < MAX_ARGS && args[i] != nullptr; i++) {
        const char* arg = args[i];
        while ('[' == *arg || '-' == *arg) {
            arg++;
        }
        string argName = "";
        for (; isalpha(*arg); arg++) {
            argName += *arg;
        }
        argNames.push_back(argName);
    }

    return argNames;
}

string CmdBase::formatAmount(const string& assetType, int64_t amount) const
{
    if (OT_ERROR_AMOUNT == amount) {
        // this probably should not happen
        return "UNKNOWN_AMOUNT";
    }

    if ("" == assetType) {
        // just return unformatted
        return to_string(amount);
    }

    return SwigWrap::FormatAmount(assetType, amount);
}

Category CmdBase::getCategory() const { return category; }

const char* CmdBase::getCommand() const { return command; }

const char* CmdBase::getHelp() const { return help; }

string CmdBase::getAccountAssetType(const string& myacct) const
{
    string assetType =
        SwigWrap::GetAccountWallet_InstrumentDefinitionID(myacct);
    if ("" == assetType) {
        otOut << "Error: cannot load instrument definition from myacct.\n";
    }
    return assetType;
}

string CmdBase::getOption(string optionName) const
{
    auto result = options.find(optionName);
    if (result == options.end()) {
        otWarn << "Option " << optionName << " not found.\n";
        return "";
    }

    otInfo << "Option  " << result->first << ": " << result->second << "\n";
    return result->second;
}

// GET PAYMENT INSTRUMENT (from payments inbox, by index.)
//
std::string CmdBase::get_payment_instrument(
    const std::string& notaryID,
    const std::string& nymID,
    std::int32_t nIndex,
    const std::string& PRELOADED_INBOX) const
{
    std::string strInstrument;
    std::string strInbox =
        VerifyStringVal(PRELOADED_INBOX)
            ? PRELOADED_INBOX
            : OT::App().API().Exec().LoadPaymentInbox(
                  notaryID, nymID);  // Returns nullptr, or an inbox.

    if (!VerifyStringVal(strInbox)) {
        otWarn << "\n\n get_payment_instrument:  "
                  "OT_API_LoadPaymentInbox Failed. (Probably just "
                  "doesn't exist yet.)\n\n";
        return "";
    }

    std::int32_t nCount = OT::App().API().Exec().Ledger_GetCount(
        notaryID, nymID, nymID, strInbox);
    if (0 > nCount) {
        otOut
            << "Unable to retrieve size of payments inbox ledger. (Failure.)\n";
        return "";
    }
    if (nIndex > (nCount - 1)) {
        otOut << "Index " << nIndex
              << " out of bounds. (The last index is: " << (nCount - 1)
              << ". The first is 0.)\n";
        return "";
    }

    strInstrument = OT::App().API().Exec().Ledger_GetInstrument(
        notaryID, nymID, nymID, strInbox, nIndex);
    if (!VerifyStringVal(strInstrument)) {
        otOut << "Failed trying to get payment instrument from payments box.\n";
        return "";
    }

    return strInstrument;
}

string CmdBase::getUsage() const
{
    stringstream ss;

    // construct usage string
    ss << "Usage:   " << command;
    for (int i = 0; i < MAX_ARGS && args[i] != nullptr; i++) {
        ss << " " << args[i];
    }
    ss << "\n\n" << help << "\n\n";
    if (usage != nullptr) {
        ss << usage << "\n\n";
    }

    return ss.str();
}

OTWallet* CmdBase::getWallet() const
{
    OTWallet* wallet = OT::App().API().OTAPI().GetWallet();
    OT_ASSERT_MSG(wallet != nullptr, "Cannot load wallet->\n");
    return wallet;
}

int32_t CmdBase::harvestTxNumbers(const string& contract, const string& mynym)
{
    SwigWrap::Msg_HarvestTransactionNumbers(
        contract, mynym, false, false, false, false, false);
    return -1;
}

string CmdBase::inputLine() { return OT_CLI_ReadLine(); }

string CmdBase::inputText(const char* what)
{
    cout << "Please paste " << what << ",\n"
         << "followed by an EOF or a ~ on a line by itself:\n";

    string input = OT_CLI_ReadUntilEOF();
    if ("" == input) {
        otOut << "Error: you did not paste " << what << ".\n";
    }
    return input;
}

#if OT_CASH
// LOAD MINT (from local storage)
//
// To load a mint withOUT retrieving it from server, call:
//
// var strMint = OT_API_LoadMint(notaryID, instrumentDefinitionID);
// It returns the mint, or null.
// LOAD MINT (from local storage).
// Also, if necessary, RETRIEVE it from the server first.
//
// Returns the mint, or null.
//
std::string CmdBase::load_or_retrieve_mint(
    const std::string& notaryID,
    const std::string& nymID,
    const std::string& instrumentDefinitionID) const
{
    std::string response = check_nym(notaryID, nymID, nymID);

    if (1 != VerifyMessageSuccess(response)) {
        otOut << "OT_ME_load_or_retrieve_mint: Cannot verify nym for IDs: \n";
        otOut << "  Notary ID: " << notaryID << "\n";
        otOut << "     Nym ID: " << nymID << "\n";
        otOut << "   Instrument Definition Id: " << instrumentDefinitionID
              << "\n";
        return "";
    }

    // HERE, WE MAKE SURE WE HAVE THE PROPER MINT...
    //
    // Download the public mintfile if it's not there, or if it's expired.
    // Also load it up into memory as a std::string (just to make sure it
    // works.)

    // expired or missing.
    if (!SwigWrap::Mint_IsStillGood(notaryID, instrumentDefinitionID)) {
        otWarn << "OT_ME_load_or_retrieve_mint: Mint file is "
                  "missing or expired. Downloading from "
                  "server...\n";

        response = OT::App()
                       .API()
                       .ServerAction()
                       .DownloadMint(
                           Identifier(notaryID),
                           Identifier(nymID),
                           Identifier(instrumentDefinitionID))
                       ->Run();

        if (1 != VerifyMessageSuccess(response)) {
            otOut << "OT_ME_load_or_retrieve_mint: Unable to "
                     "retrieve mint for IDs: \n";
            otOut << "  Notary ID: " << notaryID << "\n";
            otOut << "     Nym ID: " << nymID << "\n";
            otOut << "   Instrument Definition Id: " << instrumentDefinitionID
                  << "\n";
            return "";
        }

        if (!SwigWrap::Mint_IsStillGood(notaryID, instrumentDefinitionID)) {
            otOut << "OT_ME_load_or_retrieve_mint: Retrieved "
                     "mint, but still 'not good' for IDs: \n";
            otOut << "  Notary ID: " << notaryID << "\n";
            otOut << "     Nym ID: " << nymID << "\n";
            otOut << "   Instrument Definition Id: " << instrumentDefinitionID
                  << "\n";
            return "";
        }
    }
    // else // current mint IS available already on local storage (and not
    // expired.)

    // By this point, the mint is definitely good, whether we had to download it
    // or not.
    // It's here, and it's NOT expired. (Or we would have returned already.)

    std::string strMint = SwigWrap::LoadMint(notaryID, instrumentDefinitionID);
    if (!VerifyStringVal(strMint)) {
        otOut << "OT_ME_load_or_retrieve_mint: Unable to load mint for IDs: \n";
        otOut << "  Notary ID: " << notaryID << "\n";
        otOut << "     Nym ID: " << nymID << "\n";
        otOut << "   Instrument Definition Id: " << instrumentDefinitionID
              << "\n";
    }

    return strMint;
}
#endif  // OT_CASH

int32_t CmdBase::processResponse(const string& response, const char* what) const
{
    switch (responseStatus(response)) {
        case 1:
            break;

        case 0:
            otOut << "Error: failed to " << what << ".\n";
            return -1;

        default:
            otOut << "Error: cannot " << what << ".\n";
            return -1;
    }

    cout << response << "\n";
    return 1;
}

int32_t CmdBase::processTxResponse(
    const string& server,
    const string& mynym,
    const string& myacct,
    const string& response,
    const char* what) const
{
    if (1 != responseStatus(response)) {
        otOut << "Error: cannot " << what << ".\n";
        return -1;
    }

    if (1 != VerifyMsgBalanceAgrmntSuccess(server, mynym, myacct, response)) {
        otOut << "Error: " << what << " balance agreement failed.\n";
        return -1;
    }

    if (1 != VerifyMsgTrnxSuccess(server, mynym, myacct, response)) {
        otOut << "Error: " << what << " transaction failed.\n";
        return -1;
    }

    cout << response << "\n";

    return 1;
}

int32_t CmdBase::responseReply(
    const string& response,
    const string& server,
    const string& mynym,
    const string& myacct,
    const char* function) const
{
    return InterpretTransactionMsgReply(
        server, mynym, myacct, function, response);
}

int32_t CmdBase::responseStatus(const string& response) const
{
    return VerifyMessageSuccess(response);
}

bool CmdBase::run(const map<string, string>& _options)
{
    options = _options;
    int32_t returnValue = runWithOptions();
    options.clear();

    switch (returnValue) {
        case 0:  // no action performed, return success
            return true;
        case 1:  // success
            return true;
        case -1:  // failed
            return false;
        default:
            otOut << "Error: undefined error code: " << returnValue << ".\n";
            break;
    }

    return false;
}

std::string CmdBase::stat_asset_account(const std::string& ACCOUNT_ID) const
{
    std::string strNymID = SwigWrap::GetAccountWallet_NymID(ACCOUNT_ID);

    if (!VerifyStringVal(strNymID)) {
        otOut << "\nstat_asset_account: Cannot find account wallet for: "
              << ACCOUNT_ID << "\n";
        return "";
    }

    std::string strInstrumentDefinitionID =
        SwigWrap::GetAccountWallet_InstrumentDefinitionID(ACCOUNT_ID);

    if (!VerifyStringVal(strInstrumentDefinitionID)) {
        otOut << "\nstat_asset_account: Cannot cannot determine instrument "
                 "definition for: "
              << ACCOUNT_ID << "\n";
        return "";
    }

    std::string strName = SwigWrap::GetAccountWallet_Name(ACCOUNT_ID);
    std::string strNotaryID = SwigWrap::GetAccountWallet_NotaryID(ACCOUNT_ID);
    std::int64_t lBalance = SwigWrap::GetAccountWallet_Balance(ACCOUNT_ID);
    std::string strAssetTypeName =
        SwigWrap::GetAssetType_Name(strInstrumentDefinitionID);
    std::string strNymName = SwigWrap::GetNym_Name(strNymID);
    std::string strServerName = SwigWrap::GetServer_Name(strNotaryID);

    return "   Balance: " +
           SwigWrap::FormatAmount(strInstrumentDefinitionID, lBalance) +
           "   (" + strName + ")\nAccount ID: " + ACCOUNT_ID + " ( " + strName +
           " )\nAsset Type: " + strInstrumentDefinitionID + " ( " +
           strAssetTypeName + " )\nOwner Nym : " + strNymID + " ( " +
           strNymName + " )\nServer    : " + strNotaryID + " ( " +
           strServerName + " )";
}

vector<string> CmdBase::tokenize(const string& str, char delim, bool noEmpty)
    const
{
    vector<string> tokens;

    const char* p = str.c_str();
    int begin = 0;
    while (true) {
        int next = begin;
        while (p[next] != delim && '\0' != p[next]) {
            next++;
        }
        if (next != begin || !noEmpty) {
            tokens.push_back(str.substr(begin, next - begin));
        }
        if ('\0' == p[next]) {
            break;
        }
        begin = next + 1;
    }

    return tokens;
}
