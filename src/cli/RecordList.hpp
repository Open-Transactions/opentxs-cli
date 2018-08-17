// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLI_SRC_CLI_RECORDLIST_HPP
#define OPENTXS_CLI_SRC_CLI_RECORDLIST_HPP

#include <opentxs/opentxs.hpp>

#include "Record.hpp"

#include <cstdint>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <mutex>

namespace opentxs::cli
{
typedef std::weak_ptr<Record> weak_ptr_Record;
typedef std::shared_ptr<Record> shared_ptr_Record;

typedef std::vector<shared_ptr_Record> vec_RecordList;
typedef std::list<std::string> list_of_strings;
typedef std::map<std::string, std::string> map_of_strings;

class RecordList
{
public:
    enum ItemType { typeBoth = 0, typeTransfers = 1, typeReceipts = 2 };

    const api::client::Manager& client_;

    EXPORT static bool accept_from_paymentbox(
        const std::string& transport_notary,
        const std::string& myacct,
        const std::string& indices,
        const std::string& paymentType,
        std::string* pOptionalOutput = nullptr);

    EXPORT static bool checkIndicesRange(
        const char* name,
        const std::string& indices,
        std::int32_t items);

    EXPORT static std::string get_payment_instrument(
        const std::string& notaryID,
        const std::string& nymID,
        std::int32_t nIndex,
        const std::string& PRELOADED_INBOX);

    EXPORT static std::int32_t confirm_payment_plan(
        const std::string& server,
        const std::string& mynym,
        const std::string& myacct,
        const std::string& hisnym,
        const std::string& instrument,
        std::int32_t index,
        std::string* pOptionalOutput = nullptr);

    EXPORT static std::int32_t confirmPaymentPlan_lowLevel(
        const std::string& mynym,
        const std::string& myacct,
        const std::string& plan,
        std::string* pOptionalOutput = nullptr);

    EXPORT static std::int32_t processPayment(
        const std::string& transport_notary,
        const std::string& myacct,
        const std::string& paymentType,
        const std::string& inbox,
        const std::int32_t index,
        std::string* pOptionalOutput = nullptr,
        bool CLI_input_allowed = false);

    EXPORT static std::int32_t depositCheque(
        const std::string& server,
        const std::string& myacct,
        const std::string& mynym,
        const std::string& instrument,
        std::string* pOptionalOutput = nullptr);

    EXPORT static std::int32_t acceptFromInbox(
        const std::string& myacct,
        const std::string& indices,
        const std::int32_t itemTypeFilter);

    EXPORT static bool checkMandatory(
        const char* name,
        const std::string& value);

    EXPORT static bool checkNym(
        const char* name,
        std::string& nym,
        bool checkExistance = true);
    EXPORT static bool checkIndices(
        const char* name,
        const std::string& indices);
    EXPORT static bool checkAccount(const char* name, std::string& account);

    EXPORT static bool checkServer(const char* name, std::string& server);

    EXPORT static std::int32_t discard_incoming_payments(
        const std::string& transportNotaryId,
        const std::string& mynym,
        const std::string& indices);

    EXPORT static std::int32_t cancel_outgoing_payments(
        const std::string& mynym,
        const std::string& myacct,
        const std::string& indices);

    EXPORT static std::string inputText(const char* what);

    EXPORT static const char* textTo() { return s_strTextTo.c_str(); }
    EXPORT static const char* textFrom() { return s_strTextFrom.c_str(); }

    EXPORT static void setTextTo(std::string text) { s_strTextTo = text; }
    EXPORT static void setTextFrom(std::string text) { s_strTextFrom = text; }

    EXPORT void SetFastMode() { m_bRunFast = true; }
    EXPORT void IgnoreMail(bool bIgnore = true) { m_bIgnoreMail = bIgnore; }
    // SETUP:
    /** Set the default server here. */
    EXPORT void SetNotaryID(std::string str_id);
    /** Unless you have many servers, then use this. */
    EXPORT void AddNotaryID(std::string str_id);
    /** Also clears m_contents */
    EXPORT void ClearServers();
    EXPORT void SetInstrumentDefinitionID(std::string str_id);
    EXPORT void AddInstrumentDefinitionID(std::string str_id);
    /** Also clears m_contents */
    EXPORT void ClearAssets();
    EXPORT void SetNymID(std::string str_id);
    EXPORT void AddNymID(std::string str_id);
    /** Also clears m_contents */
    EXPORT void ClearNyms();
    EXPORT void SetAccountID(std::string str_id);
    EXPORT void AddAccountID(std::string str_id);
    /** Also clears m_contents */
    EXPORT void ClearAccounts();
    EXPORT const list_of_strings& GetNyms() const;
    EXPORT void AcceptChequesAutomatically(bool bVal = true);
    EXPORT void AcceptReceiptsAutomatically(bool bVal = true);
    EXPORT void AcceptTransfersAutomatically(bool bVal = true);
    EXPORT void AcceptCashAutomatically(bool bVal = true);
    EXPORT bool DoesAcceptChequesAutomatically() const;
    EXPORT bool DoesAcceptReceiptsAutomatically() const;
    EXPORT bool DoesAcceptTransfersAutomatically() const;
    EXPORT bool DoesAcceptCashAutomatically() const;
    /** Before populating, process out any items we're supposed to accept
     * automatically. POPULATE: */
    EXPORT bool PerformAutoAccept();
    EXPORT void notifyOfSuccessfulNotarization(
        const std::string& str_acct_id,
        const std::string p_nym_id,
        const std::string p_msg_notary_id,
        const std::string p_pmnt_notary_id,
        const std::string p_txn_contents,
        TransactionNumber lTransactionNum,
        TransactionNumber lTransNumForDisplay) const;
    /** Populates m_contents from OT API. Calls ClearContents(). */
    EXPORT bool Populate();
    /** Clears m_contents (NOT nyms, accounts, servers, or instrument
     * definitions.) */
    EXPORT void ClearContents();
    /** Populate already sorts. But if you have to add some external records
     * after Populate, then you can sort again. P.S. sorting is performed based
     * on the "from" date. */
    EXPORT void SortRecords();
    /** Let's say you also want to add some Bitmessages. (Or any other external
     * source.) This is where you do that. Make sure to call Populate, then use
     * AddSpecialMsg a few times, then call SortRecords. */
    EXPORT void AddSpecialMsg(
        const std::string& str_msg_id,  // The ID of this message, from whatever
                                        // system it came from.
        bool bIsOutgoing,
        std::int32_t nMethodID,
        const std::string& str_contents,  // Make sure to concatentate subject
                                          // with contents, before passing here.
        const std::string& str_address,
        const std::string& str_other_address,
        const std::string& str_type,
        const std::string& str_type_display,
        std::string str_my_nym_id = "",
        time64_t tDate = OT_TIME_ZERO);
    // RETRIEVE:
    EXPORT std::int32_t size() const;
    EXPORT Record GetRecord(std::int32_t nIndex);
    EXPORT bool RemoveRecord(std::int32_t nIndex);

    EXPORT RecordList();

    EXPORT ~RecordList() = default;

private:
    const api::Wallet& wallet_;
    // Defaults to false. If you set it true, it will run a lot faster. (And
    // give you less data.)
    bool m_bRunFast{false};
    bool m_bAutoAcceptCheques{false};  // Cheques and vouchers, NOT invoices.
    bool m_bAutoAcceptReceipts{false};
    bool m_bAutoAcceptTransfers{false};
    bool m_bAutoAcceptCash{false};
    bool m_bIgnoreMail{false};
    static std::string s_strTextTo;    // "To: "
    static std::string s_strTextFrom;  // "From: "
    list_of_strings m_servers;
    map_of_strings m_assets;  // <instrument_definition_id, asset_name>
    list_of_strings m_accounts;
    list_of_strings m_nyms;
    vec_RecordList m_contents;
    static const std::string s_blank;
    static const std::string s_message_type;
};
}  // namespace opentxs::cli
#endif  // OPENTXS_CLI_SRC_CLI_RECORDLIST_HPP
