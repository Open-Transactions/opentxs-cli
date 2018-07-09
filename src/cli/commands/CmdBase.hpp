/************************************************************
 *
 *  CmdBase.hpp
 *
 */

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

#ifndef OPENTXS_CLIENT_CMDBASE_HPP
#define OPENTXS_CLIENT_CMDBASE_HPP

#include <opentxs/opentxs.hpp>

#include <vector>
#include <map>
#include <mutex>

// #ifndef SWIG
// #include "opentxs/core/OTStorage.hpp"
// #endif

#ifndef SWIG
class the_lambda_struct
{
public:
    std::vector<std::string> the_vector;  // used for returning a list of
                                          // something.
    std::string the_asset_acct;     // for newoffer, we want to remove existing
                                    // offers
                                    // for the same accounts in certain cases.
    std::string the_currency_acct;  // for newoffer, we want to remove existing
                                    // offers
                                    // for the same accounts in certain cases.
    std::string the_scale;          // for newoffer as well.
    std::string the_price;          // for newoffer as well.
    bool bSelling{false};           // for newoffer as well.

    the_lambda_struct();
};
typedef std::map<std::string, opentxs::OTDB::OfferDataNym*> SubMap;
typedef std::map<std::string, SubMap*> MapOfMaps;
typedef std::int32_t (*LambdaFunc)(
    const opentxs::OTDB::OfferDataNym& offer_data,
    std::int32_t nIndex,
    const MapOfMaps& map_of_maps,
    const SubMap& sub_map,
    the_lambda_struct& extra_vals);

MapOfMaps* convert_offerlist_to_maps(opentxs::OTDB::OfferListNym& offerList);
std::int32_t find_strange_offers(
    const opentxs::OTDB::OfferDataNym& offer_data,
    std::int32_t nIndex,
    const MapOfMaps& map_of_maps,
    const SubMap& sub_map,
    the_lambda_struct& extra_vals);  // if 10 offers are printed
                                     // for the SAME market,
                                     // nIndex will be 0..9
std::int32_t iterate_nymoffers_maps(
    MapOfMaps& map_of_maps,
    LambdaFunc the_lambda);  // low level. map_of_maps
                             // must be
                             // good. (assumed.)
std::int32_t iterate_nymoffers_maps(
    MapOfMaps& map_of_maps,
    LambdaFunc the_lambda,
    the_lambda_struct& extra_vals);  // low level.
                                     // map_of_maps
                                     // must be good.
                                     // (assumed.)
std::int32_t iterate_nymoffers_sub_map(
    const MapOfMaps& map_of_maps,
    SubMap& sub_map,
    LambdaFunc the_lambda,
    the_lambda_struct& extra_vals);
opentxs::OTDB::OfferListNym* loadNymOffers(
    const std::string& notaryID,
    const std::string& nymID);
std::int32_t output_nymoffer_data(
    const opentxs::OTDB::OfferDataNym& offer_data,
    std::int32_t nIndex,
    const MapOfMaps& map_of_maps,
    const SubMap& sub_map,
    the_lambda_struct& extra_vals);  // if 10 offers are printed
                                     // for the SAME market,
                                     // nIndex will be 0..9
#endif

namespace opentxs
{

class OTWallet;

const int MAX_ARGS = 10;

typedef enum {
    catError = 0,
    catAdmin = 1,
    catWallet = 2,
    catMisc = 3,
    catMarkets = 4,
    catAccounts = 5,
    catOtherUsers = 6,
    catInstruments = 7,
    catBaskets = 8,
    catNyms = 9,
    catBlockchain = 10,
    catLast = 11
} Category;

class CmdBase
{
public:
    EXPORT CmdBase();
    virtual ~CmdBase();

    EXPORT const std::vector<std::string>& extractArgumentNames();
    EXPORT Category getCategory() const;
    EXPORT const char* getCommand() const;
    EXPORT const char* getHelp() const;
    virtual std::string getUsage() const;
    EXPORT bool run(const std::map<std::string, std::string>& _options);

protected:
    const char* args[MAX_ARGS];
    Category category;
    const char* command;
    const char* help;
    const char* usage;

    std::string check_nym(
        const std::string& NOTARY_ID,
        const std::string& NYM_ID,
        const std::string& TARGET_NYM_ID) const;

    std::string stat_asset_account(const std::string& ACCOUNT_ID) const;
    bool checkAccount(const char* name, std::string& account) const;
    int64_t checkAmount(
        const char* name,
        const std::string& amount,
        const std::string& myacct) const;
    bool checkBoolean(const char* name, const std::string& value) const;
    bool checkFlag(const char* name, const std::string& value) const;
    int32_t checkIndex(
        const char* name,
        const std::string& index,
        int32_t items) const;
    bool checkIndices(const char* name, const std::string& indices) const;
    bool checkIndicesRange(
        const char* name,
        const std::string& indices,
        int32_t items) const;
    bool checkMandatory(const char* name, const std::string& value) const;
    bool checkNym(
        const char* name,
        std::string& nym,
        bool checkExistance = true) const;
    bool checkPurse(const char* name, std::string& purse) const;
    bool checkServer(const char* name, std::string& server) const;
    int64_t checkTransNum(const char* name, const std::string& id) const;
    bool checkValue(const char* name, const std::string& index) const;
    void dashLine() const;
    std::string formatAmount(const std::string& assetType, int64_t amount)
        const;
    std::string getAccountAssetType(const std::string& myacct) const;
    std::string getOption(std::string optionName) const;
    OTWallet* getWallet() const;
    int32_t harvestTxNumbers(
        const std::string& contract,
        const std::string& mynym);
    std::string inputLine();
    std::string inputText(const char* what);
    int32_t processResponse(const std::string& response, const char* what)
        const;
    int32_t processTxResponse(
        const std::string& server,
        const std::string& mynym,
        const std::string& myacct,
        const std::string& response,
        const char* what) const;
    int32_t responseReply(
        const std::string& response,
        const std::string& server,
        const std::string& mynym,
        const std::string& myacct,
        const char* function) const;
    int32_t responseStatus(const std::string& response) const;
    virtual int32_t runWithOptions() = 0;
    std::vector<std::string> tokenize(
        const std::string& str,
        char delim,
        bool noEmpty) const;

private:
    std::vector<std::string> argNames;
    std::map<std::string, std::string> options;
};

}  // namespace opentxs

#endif  // OPENTXS_CLIENT_CMDBASE_HPP
