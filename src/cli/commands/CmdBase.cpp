// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdBase.hpp"

#include <opentxs/opentxs.hpp>

#include <ctype.h>
#include <stdint.h>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#define OT_METHOD "opentxs::CmdBase"

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
                LogNormal(OT_METHOD)(__FUNCTION__)(strLocation)(
                    ": Unable to reference (nym) offerData "
                    "on offerList, at index: ")(nIndex)(".")
                    .Flush();
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
                LogDetail(OT_METHOD)(__FUNCTION__)(
                    ": The sub-map already exists!")
                    .Flush();

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
                LogDetail(OT_METHOD)(__FUNCTION__)(
                    ": The sub-map does NOT already exist!")
                    .Flush();
                //
                // Let's create the submap with this new offer, and add it
                // to the main map.
                //
                sub_map = new SubMap;
                (*sub_map)[strTransactionID] = &offerData;

                if (nullptr == map_of_maps) { map_of_maps = new MapOfMaps; }

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
    LogTrace(strLocation)(": About to compare the new potential offer against "
                          "one of the existing ones...")
        .Flush();

    if ((extra_vals.the_asset_acct == offer_data.asset_acct_id) &&
        (extra_vals.the_currency_acct == offer_data.currency_acct_id) &&
        (extra_vals.the_scale == offer_data.scale)) {
        LogTrace(strLocation)(": the account IDs and the scale match...")
            .Flush();

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
        LogNormal(OT_METHOD)(__FUNCTION__)(strLocation)(
            ": No range retrieved from sub_map. It must be "
            "non-existent, I guess.")
            .Flush();
        return -1;
    }
    if (sub_map.empty()) {
        // Should never happen since we already made sure all the sub_maps
        // have data on them. Therefore if this range is empty now, it's a
        // chaiscript
        // bug (extremely unlikely.)
        //
        LogNormal(OT_METHOD)(__FUNCTION__)(strLocation)(
            ": Error: A range was retrieved for the "
            "sub_map, but the range is empty.")
            .Flush();
        return -1;
    }

    std::int32_t nIndex = -1;
    for (auto it = sub_map.begin(); it != sub_map.end(); ++it) {
        ++nIndex;
        // var offer_data_pair = range_sub_map.front();

        if (nullptr == it->second) {
            LogNormal(OT_METHOD)(__FUNCTION__)(strLocation)(
                ": Looping through range_sub_map range, "
                "and first offer_data_pair fails to "
                "verify.")
                .Flush();
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
            LogNormal(OT_METHOD)(__FUNCTION__)(strLocation)(
                ": Error: the_lambda failed.")
                .Flush();
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
        LogNormal(OT_METHOD)(__FUNCTION__)(strLocation)(
            ": No range retrieved from map_of_maps.")
            .Flush();
        return -1;
    }
    if (map_of_maps.empty()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(strLocation)(
            ": A range was retrieved for the map_of_maps, "
            "but the range is empty.")
            .Flush();
        return -1;
    }

    for (auto it = map_of_maps.begin(); it != map_of_maps.end(); ++it) {
        // var sub_map_pair = range_map_of_maps.front();
        if (nullptr == it->second) {
            LogNormal(OT_METHOD)(__FUNCTION__)(strLocation)(
                ": Looping through map_of_maps range, and "
                "first sub_map_pair fails to verify.")
                .Flush();
            return -1;
        }

        std::string strMapKey = it->first;

        SubMap& sub_map = *it->second;
        if (sub_map.empty()) {
            LogNormal(OT_METHOD)(__FUNCTION__)(strLocation)(
                ": Error: Sub_map is empty (Then how is it "
                "even here?? Submaps are only added based "
                "on existing offers.")
                .Flush();
            return -1;
        }

        std::int32_t nSubMap = iterate_nymoffers_sub_map(
            map_of_maps, sub_map, the_lambda, extra_vals);
        if (-1 == nSubMap) {
            LogNormal(OT_METHOD)(__FUNCTION__)(strLocation)(
                ": Error: while trying "
                "to iterate_nymoffers_sub_map.")
                .Flush();
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

    if (OTDB::Exists(
            Opentxs::Client().DataFolder(),
            "nyms",
            notaryID,
            "offers",
            nymID + ".bin")) {
        LogDetail(OT_METHOD)(__FUNCTION__)(
            ": Offers file exists... Querying nyms...")
            .Flush();
        OTDB::Storable* storable = OTDB::QueryObject(
            OTDB::STORED_OBJ_OFFER_LIST_NYM,
            Opentxs::Client().DataFolder(),
            "nyms",
            notaryID,
            "offers",
            nymID + ".bin");

        if (nullptr == storable) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Unable to verify storable object. "
                "Probably doesn't exist.")
                .Flush();
            return nullptr;
        }

        LogDetail(OT_METHOD)(__FUNCTION__)(
            ": QueryObject worked. Now dynamic casting from storable to a "
            "(nym) offerList...")
            .Flush();
        offerList = dynamic_cast<OTDB::OfferListNym*>(storable);

        if (nullptr == offerList) {
            LogNormal(OT_METHOD)(__FUNCTION__)(": Unable to dynamic cast a "
                                               "storable to a (nym) offerList.")
                .Flush();
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
       LogNormal(OT_METHOD)(__FUNCTION__)(": Scale:")(
	   strScale)(".").Flush();
        LogNormal(OT_METHOD)(__FUNCTION__)(": Asset:")(
           strInstrumentDefinitionID)(".")
           .Flush();
        LogNormal(OT_METHOD)(__FUNCTION__)(": Currency:")(
	   strCurrencyTypeID)(".")
           .Flush();
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Index/Trans#/Type/Price/Available")
            .Flush();
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
    for (int i = 0; i < MAX_ARGS; i++) { args[i] = nullptr; }
}

CmdBase::~CmdBase() {}

// CHECK USER (download a public key)
//
std::string CmdBase::check_nym(
    const std::string& notaryID,
    const std::string& nymID,
    const std::string& targetNymID) const
{
    auto action = Opentxs::Client().ServerAction().DownloadNym(
        Identifier::Factory(nymID),
        Identifier::Factory(notaryID),
        Identifier::Factory(targetNymID));

    return action->Run();
}

bool CmdBase::checkAccount(const char* name, string& account) const
{
    return opentxs::cli::RecordList::checkAccount(name, account);
}

int64_t CmdBase::checkAmount(
    const char* name,
    const string& amount,
    const string& myacct) const
{
    if (!checkMandatory(name, amount)) { return OT_ERROR_AMOUNT; }

    string assetType = getAccountAssetType(myacct);
    if ("" == assetType) { return OT_ERROR_AMOUNT; }

    int64_t value = SwigWrap::StringToAmount(assetType, amount);
    if (OT_ERROR_AMOUNT == value) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: ")(name)(
            ": invalid amount: ")(amount)(".")
            .Flush();
        return OT_ERROR_AMOUNT;
    }

    return value;
}

bool CmdBase::checkBoolean(const char* name, const string& value) const
{
    if (!checkMandatory(name, value)) { return false; }
    if (value != "false" && value != "true") {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: ")(name)(
            ": expected 'false' or 'true'.")
            .Flush();
        return false;
    }

    return true;
}

bool CmdBase::checkFlag(const char* name, const string& value) const
{
    if (!checkMandatory(name, value)) { return false; }

    if (!checkBoolean(name, value)) { return false; }

    return true;
}

int32_t CmdBase::checkIndex(
    const char* name,
    const string& index,
    int32_t items) const
{
    if (!checkValue(name, index)) { return -1; }

    if (!opentxs::cli::RecordList::checkIndicesRange(name, index, items)) {
        return -1;
    }

    return stoi(index);
}

bool CmdBase::checkIndices(const char* name, const string& indices) const
{
    return opentxs::cli::RecordList::checkIndices(name, indices);
}

bool CmdBase::checkMandatory(const char* name, const string& value) const
{
    return opentxs::cli::RecordList::checkMandatory(name, value);
}

bool CmdBase::checkNym(const char* name, string& nym, bool checkExistance) const
{
    return opentxs::cli::RecordList::checkNym(name, nym, checkExistance);
}

bool CmdBase::checkPurse(const char* name, string& purse) const
{
    if (!checkMandatory(name, purse)) return false;

    OTIdentifier theID = Identifier::Factory(purse);
    ConstUnitDefinition pUnit;  // shared_ptr to const.

    // See if it's available using the full length ID.
    if (!theID->empty())
        pUnit = Opentxs::Client().Wallet().UnitDefinition(theID);

    if (!pUnit) {
        const auto units = Opentxs::Client().Wallet().UnitDefinitionList();

        // See if it's available using the partial length ID.
        for (auto& it : units) {
            if (0 == it.first.compare(0, purse.length(), purse)) {
                pUnit = Opentxs::Client().Wallet().UnitDefinition(
                    Identifier::Factory(it.first));
                break;
            }
        }
        if (!pUnit) {
            // See if it's available using the full length name.
            for (auto& it : units) {
                if (0 == it.second.compare(0, it.second.length(), purse)) {
                    pUnit = Opentxs::Client().Wallet().UnitDefinition(
                        Identifier::Factory(it.first));
                    break;
                }
            }

            if (!pUnit) {
                // See if it's available using the partial name.
                for (auto& it : units) {
                    if (0 == it.second.compare(0, purse.length(), purse)) {
                        pUnit = Opentxs::Client().Wallet().UnitDefinition(
                            Identifier::Factory(it.first));
                        break;
                    }
                }
            }
        }
    }

    if (!pUnit) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: ")(name)(
            ": unknown unit definition: ")(purse)(".")
            .Flush();
        return false;
    }

    purse = pUnit->ID()->str();
    LogNormal(OT_METHOD)(__FUNCTION__)(": Using ")(
       name)(": ")(purse)(".").Flush();
    return true;
}

bool CmdBase::checkServer(const char* name, string& server) const
{
    return opentxs::cli::RecordList::checkServer(name, server);
}

int64_t CmdBase::checkTransNum(const char* name, const string& id) const
{
    if (!checkMandatory(name, id)) { return -1; }

    for (string::size_type i = 0; i < id.length(); i++) {
        if (!isdigit(id[i])) {
            LogNormal(OT_METHOD)(__FUNCTION__)(": Error: ")(name)(
                ": not a value: ")(id)(".")
                .Flush();
            return -1;
        }
    }

    int64_t value = stoll(id);
    if (0 >= value) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: ")(name)(
            ": invalid value: ")(id)(".")
            .Flush();
        return -1;
    }

    return value;
}

bool CmdBase::checkValue(const char* name, const string& value) const
{
    if (!checkMandatory(name, value)) { return false; }

    for (string::size_type i = 0; i < value.length(); i++) {
        if (!isdigit(value[i])) {
            LogNormal(OT_METHOD)(__FUNCTION__)(": Error: ")(name)(
                ": not a value: ")(value)(".")
                .Flush();
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
    if (0 != argNames.size()) { return argNames; }

    // extract argument names from usage help text
    for (int i = 0; i < MAX_ARGS && args[i] != nullptr; i++) {
        const char* arg = args[i];
        while ('[' == *arg || '-' == *arg) { arg++; }
        string argName = "";
        for (; isalpha(*arg); arg++) { argName += *arg; }
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
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot load instrument definition from myacct.")
            .Flush();
    }
    return assetType;
}

string CmdBase::getOption(string optionName) const
{
    auto result = options.find(optionName);

    if (result == options.end()) {
        LogDetail(OT_METHOD)(__FUNCTION__)(": Option ")(optionName)(
            " not found.")
            .Flush();
        return "";
    }

    LogVerbose("Option  ")(result->first)(": ")(result->second).Flush();

    return result->second;
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
    if (usage != nullptr) { ss << usage << "\n\n"; }

    return ss.str();
}

OTWallet* CmdBase::getWallet() const
{
    OTWallet* wallet = Opentxs::Client().OTAPI().GetWallet();
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
        LogNormal(OT_METHOD)(__FUNCTION__)(
	   ": Error: you did not paste ")(what)(".")
            .Flush();
    }
    return input;
}

int32_t CmdBase::processResponse(const string& response, const char* what) const
{
    switch (responseStatus(response)) {
        case 1:
            break;

        case 0:
            LogNormal(OT_METHOD)(__FUNCTION__)(": Error: failed to ")(what)(".")
                .Flush();
            return -1;

        default:
            LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot ")(what)(".")
                .Flush();
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
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot ")(what)(".")
            .Flush();
        return -1;
    }

    if (1 != VerifyMsgBalanceAgrmntSuccess(
                 Opentxs::Client(), server, mynym, myacct, response)) {

        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: ")(what)(
            " balance agreement failed.")
            .Flush();
        return -1;
    }

    if (1 != VerifyMsgTrnxSuccess(
                 Opentxs::Client(), server, mynym, myacct, response)) {

        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: ")(what)(
            " transaction failed.")
            .Flush();
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
        Opentxs::Client(), server, mynym, myacct, function, response);
}

int32_t CmdBase::responseStatus(const string& response) const
{
    return VerifyMessageSuccess(Opentxs::Client(), response);
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
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: undefined error code: ")(returnValue)(".")
                .Flush();
            break;
    }

    return false;
}

std::string CmdBase::stat_asset_account(const std::string& ACCOUNT_ID) const
{
    std::string strNymID = SwigWrap::GetAccountWallet_NymID(ACCOUNT_ID);

    if (!VerifyStringVal(strNymID)) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": stat_asset_account: Cannot find account wallet for: ")(
            ACCOUNT_ID)(".")
            .Flush();
        return "";
    }

    std::string strInstrumentDefinitionID =
        SwigWrap::GetAccountWallet_InstrumentDefinitionID(ACCOUNT_ID);

    if (!VerifyStringVal(strInstrumentDefinitionID)) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": stat_asset_account: Cannot cannot determine instrument "
            "definition for: ")(ACCOUNT_ID)(".")
            .Flush();
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
        while (p[next] != delim && '\0' != p[next]) { next++; }
        if (next != begin || !noEmpty) {
            tokens.push_back(str.substr(begin, next - begin));
        }
        if ('\0' == p[next]) { break; }
        begin = next + 1;
    }

    return tokens;
}
