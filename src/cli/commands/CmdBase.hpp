/************************************************************
 *
 *  CmdBase.hpp
 *
 */

/************************************************************
 -----BEGIN PGP SIGNED MESSAGE-----
 Hash: SHA1

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
 *  Copyright (C) 2010-2013 by "Fellow Traveler" (A pseudonym)
 *
 *  EMAIL:
 *  FellowTraveler@rayservers.net
 *
 *  BITCOIN:  1NtTPVVjDsUfDWybS4BwvHpG2pdS9RnYyQ
 *
 *  KEY FINGERPRINT (PGP Key in license file):
 *  9DD5 90EB 9292 4B48 0484  7910 0308 00ED F951 BB8E
 *
 *  OFFICIAL PROJECT WIKI(s):
 *  https://github.com/FellowTraveler/Moneychanger
 *  https://github.com/FellowTraveler/Open-Transactions/wiki
 *
 *  WEBSITE:
 *  http://www.OpenTransactions.org/
 *
 *  Components and licensing:
 *   -- Moneychanger..A Java client GUI.....LICENSE:.....GPLv3
 *   -- otlib.........A class library.......LICENSE:...LAGPLv3
 *   -- otapi.........A client API..........LICENSE:...LAGPLv3
 *   -- opentxs/ot....Command-line client...LICENSE:...LAGPLv3
 *   -- otserver......Server Application....LICENSE:....AGPLv3
 *  Github.com/FellowTraveler/Open-Transactions/wiki/Components
 *
 *  All of the above OT components were designed and written by
 *  Fellow Traveler, with the exception of Moneychanger, which
 *  was contracted out to Vicky C (bitcointrader4@gmail.com).
 *  The open-source community has since actively contributed.
 *
 *  -----------------------------------------------------
 *
 *   LICENSE:
 *   This program is free software: you can redistribute it
 *   and/or modify it under the terms of the GNU Affero
 *   General Public License as published by the Free Software
 *   Foundation, either version 3 of the License, or (at your
 *   option) any later version.
 *
 *   ADDITIONAL PERMISSION under the GNU Affero GPL version 3
 *   section 7: (This paragraph applies only to the LAGPLv3
 *   components listed above.) If you modify this Program, or
 *   any covered work, by linking or combining it with other
 *   code, such other code is not for that reason alone subject
 *   to any of the requirements of the GNU Affero GPL version 3.
 *   (==> This means if you are only using the OT API, then you
 *   don't have to open-source your code--only your changes to
 *   Open-Transactions itself must be open source. Similar to
 *   LGPLv3, except it applies to software-as-a-service, not
 *   just to distributing binaries.)
 *
 *   Extra WAIVER for OpenSSL, Lucre, and all other libraries
 *   used by Open Transactions: This program is released under
 *   the AGPL with the additional exemption that compiling,
 *   linking, and/or using OpenSSL is allowed. The same is true
 *   for any other open source libraries included in this
 *   project: complete waiver from the AGPL is hereby granted to
 *   compile, link, and/or use them with Open-Transactions,
 *   according to their own terms, as long as the rest of the
 *   Open-Transactions terms remain respected, with regard to
 *   the Open-Transactions code itself.
 *
 *   Lucre License:
 *   This code is also "dual-license", meaning that Ben Lau-
 *   rie's license must also be included and respected, since
 *   the code for Lucre is also included with Open Transactions.
 *   See Open-Transactions/src/otlib/lucre/LUCRE_LICENSE.txt
 *   The Laurie requirements are light, but if there is any
 *   problem with his license, simply remove the Lucre code.
 *   Although there are no other blind token algorithms in Open
 *   Transactions (yet. credlib is coming), the other functions
 *   will continue to operate.
 *   See Lucre on Github:  https://github.com/benlaurie/lucre
 *   -----------------------------------------------------
 *   You should have received a copy of the GNU Affero General
 *   Public License along with this program.  If not, see:
 *   http://www.gnu.org/licenses/
 *
 *   If you would like to use this software outside of the free
 *   software license, please contact FellowTraveler.
 *   (Unfortunately many will run anonymously and untraceably,
 *   so who could really stop them?)
 *
 *   DISCLAIMER:
 *   This program is distributed in the hope that it will be
 *   useful, but WITHOUT ANY WARRANTY; without even the implied
 *   warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *   PURPOSE.  See the GNU Affero General Public License for
 *   more details.

 -----BEGIN PGP SIGNATURE-----
 Version: GnuPG v1.4.9 (Darwin)

 iQIcBAEBAgAGBQJRSsfJAAoJEAMIAO35UbuOQT8P/RJbka8etf7wbxdHQNAY+2cC
 vDf8J3X8VI+pwMqv6wgTVy17venMZJa4I4ikXD/MRyWV1XbTG0mBXk/7AZk7Rexk
 KTvL/U1kWiez6+8XXLye+k2JNM6v7eej8xMrqEcO0ZArh/DsLoIn1y8p8qjBI7+m
 aE7lhstDiD0z8mwRRLKFLN2IH5rAFaZZUvj5ERJaoYUKdn4c+RcQVei2YOl4T0FU
 LWND3YLoH8naqJXkaOKEN4UfJINCwxhe5Ke9wyfLWLUO7NamRkWD2T7CJ0xocnD1
 sjAzlVGNgaFDRflfIF4QhBx1Ddl6wwhJfw+d08bjqblSq8aXDkmFA7HeunSFKkdn
 oIEOEgyj+veuOMRJC5pnBJ9vV+7qRdDKQWaCKotynt4sWJDGQ9kWGWm74SsNaduN
 TPMyr9kNmGsfR69Q2Zq/FLcLX/j8ESxU+HYUB4vaARw2xEOu2xwDDv6jt0j3Vqsg
 x7rWv4S/Eh18FDNDkVRChiNoOIilLYLL6c38uMf1pnItBuxP3uhgY6COm59kVaRh
 nyGTYCDYD2TK+fI9o89F1297uDCwEJ62U0Q7iTDp5QuXCoxkPfv8/kX6lS6T3y9G
 M9mqIoLbIQ1EDntFv7/t6fUTS2+46uCrdZWbQ5RjYXdrzjij02nDmJAm2BngnZvd
 kamH0Y/n11lCvo1oQxM+
 =uSzz
 -----END PGP SIGNATURE-----
 **************************************************************/

#ifndef OPENTXS_CLIENT_CMDBASE_HPP
#define OPENTXS_CLIENT_CMDBASE_HPP

#ifdef ANDROID
#include <opentxs/core/util/android_string.hpp>
#endif  // ANDROID
#include <opentxs/core/util/Common.hpp>

#include <vector>
#include <map>

#ifndef SWIG
#include "opentxs/core/OTStorage.hpp"
#endif

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

    bool checkAccount(const char* name, std::string& account) const;
    int64_t checkAmount(
        const char* name,
        const std::string& amount,
        const std::string& myacct) const;
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
    std::string get_payment_instrument(
        const std::string& NOTARY_ID,
        const std::string& NYM_ID,
        std::int32_t nIndex,
        const std::string& PRELOADED_INBOX = "") const;
    OTWallet* getWallet() const;
    int32_t harvestTxNumbers(
        const std::string& contract,
        const std::string& mynym);
    std::string inputLine();
    std::string inputText(const char* what);
#if OT_CASH
    std::string load_or_retrieve_mint(
        const std::string& NOTARY_ID,
        const std::string& NYM_ID,
        const std::string& INSTRUMENT_DEFINITION_ID) const;
#endif  // OT_CASH
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
    std::string stat_asset_account(const std::string& ACCOUNT_ID) const;
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
