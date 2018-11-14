// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "RecordList.hpp"

#include <opentxs/opentxs.hpp>
#include <opentxs/client/Helpers.hpp>

#include <cinttypes>
#include <cstdint>
#include <iterator>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <utility>

#define OT_METHOD "opentxs::RecordList"

namespace
{

const std::string Instrument_TypeStrings[] = {
    // OTCheque is derived from OTTrackable, which is derived from OTInstrument,
    // which is
    // derived from OTScriptable, which is derived from Contract.
    "cheque",   // A cheque drawn on a user's account.
    "voucher",  // A cheque drawn on a server account (cashier's cheque aka
                // banker's cheque)
    "invoice",  // A cheque with a negative amount. (Depositing this causes a
                // payment out, instead of a deposit in.)
    "payment plan",    // An OTCronItem-derived OTPaymentPlan, related to a
                       // recurring payment plan.
    "smart contract",  // An OTCronItem-derived OTSmartContract, related to a
                       // smart contract.
    "cash",  // An Contract-derived OTPurse containing a list of cash OTTokens.
    "agreement",  // An transactionType::notice which could be a Payment Plan OR
                  // a
                  // Smart Contract.
    "ERROR_STATE"};

const std::string& GetTypeString(int theType)
{
    return Instrument_TypeStrings[theType];
}

}  // namespace

namespace opentxs::cli
{
RecordList::RecordList()
    : client_{Opentxs::Client()}
    , wallet_(client_.Wallet())
    , m_bRunFast(false)
    , m_bAutoAcceptCheques(false)
    , m_bAutoAcceptReceipts(false)
    , m_bAutoAcceptTransfers(false)
    , m_bAutoAcceptCash(false)
    , m_bIgnoreMail(false)
{
}

// DISPLAY FORMATTING FOR "TO:" AND "FROM:"
#define MC_UI_TEXT_TO "%s"
#define MC_UI_TEXT_FROM "%s"

//#define MC_UI_TEXT_TO "<font color='grey'>To:</font> %s"
//#define MC_UI_TEXT_FROM "<font color='grey'>From:</font> %s"

// ---------------------------
// static

const std::string RecordList::s_blank("");
const std::string RecordList::s_message_type("message");

std::string RecordList::s_strTextTo(MC_UI_TEXT_TO);      // "To: %s"
std::string RecordList::s_strTextFrom(MC_UI_TEXT_FROM);  // "From: %s"

// Set the default server here.

void RecordList::SetNotaryID(std::string str_id)
{
    ClearServers();
    AddNotaryID(str_id);
}

// Unless you have many servers, then use this.

void RecordList::AddNotaryID(std::string str_id)
{
    m_servers.insert(m_servers.end(), str_id);
}

// Also clears m_contents

void RecordList::ClearServers()
{
    ClearContents();
    m_servers.clear();
}

void RecordList::SetInstrumentDefinitionID(std::string str_id)
{
    ClearAssets();
    AddInstrumentDefinitionID(str_id);
}

void RecordList::AddInstrumentDefinitionID(std::string str_id)
{
    const auto strInstrumentDefinitionID = String::Factory(str_id);
    const auto theInstrumentDefinitionID =
        Identifier::Factory(strInstrumentDefinitionID);
    std::string str_asset_name;
    // Name is dollars, fraction is cents, TLA is USD and
    // Symbol is $ (for example.) Here, we're grabbing the TLA.
    //
    auto pUnitDefinition = wallet_.UnitDefinition(theInstrumentDefinitionID);
    // Wallet owns this object
    if (pUnitDefinition) {
        str_asset_name = pUnitDefinition->TLA();  // This might be "USD" --
                                                  // preferable that this
                                                  // works.
        if (str_asset_name.empty())
            str_asset_name =
                pUnitDefinition->GetCurrencySymbol();  // This might be "$".
        if (str_asset_name.empty())
            str_asset_name =
                pUnitDefinition->GetCurrencyName();  // This might be
                                                     // "dollars".
    }
    if (str_asset_name.empty())
        str_asset_name = SwigWrap::GetAssetType_Name(
            str_id);  // Otherwise we try to grab the name.
    // (Otherwise we just leave it blank. The ID is too big to cram in here.)
    m_assets.insert(
        std::pair<std::string, std::string>(str_id, str_asset_name));
}

void RecordList::ClearAssets()
{
    ClearContents();
    m_assets.clear();
}

void RecordList::SetNymID(std::string str_id)
{
    ClearNyms();
    AddNymID(str_id);
}

void RecordList::AddNymID(std::string str_id)
{
    m_nyms.insert(m_nyms.end(), str_id);
}

void RecordList::ClearNyms()
{
    ClearContents();
    m_nyms.clear();
}

void RecordList::SetAccountID(std::string str_id)
{
    ClearAccounts();
    AddAccountID(str_id);
}

bool RecordList::checkIndicesRange(  // static method
    const char* name,
    const std::string& indices,
    std::int32_t items)
{
    if ("all" == indices) { return true; }

    for (std::string::size_type i = 0; i < indices.length(); i++) {
        std::int32_t value = 0;
        for (; isdigit(indices[i]); i++) {
            value = value * 10 + indices[i] - '0';
        }
        if (0 > value || value >= items) {
            LogNormal(OT_METHOD)(__FUNCTION__)(":  Error: ")(name)(": value (")(
                value)(") out of range (must be < ")(items)(").")
                .Flush();
            return false;
        }
    }

    return true;
}

// GET PAYMENT INSTRUMENT (from payments inbox, by index.)
//
std::string RecordList::get_payment_instrument(  // static method
    const std::string& notaryID,
    const std::string& nymID,
    std::int32_t nIndex,
    const std::string& PRELOADED_INBOX)
{
    std::string strInstrument;
    std::string strInbox =
        VerifyStringVal(PRELOADED_INBOX)
            ? PRELOADED_INBOX
            : Opentxs::Client().Exec().LoadPaymentInbox(
                  notaryID, nymID);  // Returns nullptr, or an inbox.

    if (!VerifyStringVal(strInbox)) {
        LogDetail(OT_METHOD)(__FUNCTION__)(
            ": OT_API_LoadPaymentInbox Failed. (Probably just "
            "doesn't exist yet).")
            .Flush();
        return "";
    }

    std::int32_t nCount = Opentxs::Client().Exec().Ledger_GetCount(
        notaryID, nymID, nymID, strInbox);
    if (0 > nCount) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Unable to retrieve size of payments "
            "inbox ledger. (Failure).")
            .Flush();
        return "";
    }
    if (nIndex > (nCount - 1)) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Index ")(nIndex)(
            " out of bounds. (The last index is: ")(nCount - 1)(
            ". The first is 0).")
            .Flush();
        return "";
    }

    strInstrument = Opentxs::Client().Exec().Ledger_GetInstrument(
        notaryID, nymID, nymID, strInbox, nIndex);
    if (!VerifyStringVal(strInstrument)) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Failed trying to get payment "
                                           "instrument from payments box.")
            .Flush();
        return "";
    }

    return strInstrument;
}

bool RecordList::accept_from_paymentbox(  // static function
    const std::string& transport_notary,
    const std::string& myacct,
    const std::string& indices,
    const std::string& paymentType,
    std::string* pOptionalOutput /*=nullptr*/)
{
    if (transport_notary.empty()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: transport_notary is empty.")
            .Flush();
        return -1;
    }

    if (myacct.empty()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: myacct is empty.").Flush();
        return -1;
    }

    std::string acct_server = SwigWrap::GetAccountWallet_NotaryID(myacct);
    if (acct_server.empty()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot determine payment "
                                           "notary from myacct.")
            .Flush();
        return -1;
    }

    std::string mynym = SwigWrap::GetAccountWallet_NymID(myacct);
    if (mynym.empty()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot determine mynym from myacct.")
            .Flush();
        return -1;
    }

    std::string inbox = SwigWrap::LoadPaymentInbox(transport_notary, mynym);
    if (inbox.empty()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot load payment inbox.")
            .Flush();
        return -1;
    }

    std::int32_t items =
        SwigWrap::Ledger_GetCount(transport_notary, mynym, mynym, inbox);
    if (0 > items) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot load payment inbox item count.")
            .Flush();
        return -1;
    }

    if (!checkIndicesRange("indices", indices, items)) { return -1; }

    if (0 == items) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": The payment inbox is empty.")
            .Flush();
        return 0;
    }

    // Regarding bIsDefinitelyPaymentPlan:
    // I say "definitely" because there are cases where this could be false
    // and it's still a payment plan. For example, someone may have passed
    // "ANY" instead of "PAYMENT PLAN" -- in that case, it's still a payment
    // plan, but at this spot we just don't DEFINITELY know that yet.
    // Is that a problem? No, because processPayment can actually handle that
    // case as well. But I still prefer to handle it higher up (here) where
    // possible, so I can phase out the other. IOW, I actually want to disallow
    // processPayment from processing payment plans. You shouldn't be able
    // to agree to a long-term recurring payment plan by just accepting "all".
    // Rather, you should have to specifically look at that plan, and explicitly
    // confirm your agreement to it, before it can get activated. That's what
    // I'm enforcing here.
    //
    const bool bIsDefinitelyPaymentPlan = ("PAYMENT PLAN" == paymentType);
    const bool bIsDefinitelySmartContract = ("SMARTCONTRACT" == paymentType);

    if (bIsDefinitelySmartContract) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": accept_from_paymentbox: It's a bug that this function was even "
            "called at all! "
            "You CANNOT confirm smart contracts via this function. "
            "The reason is because you have to select various accounts "
            "during the "
            "confirmation process. The function confirmSmartContract "
            "would ask various questions "
            "at the command line about which accounts to choose. Thus, "
            "you MUST have "
            "your own code in the GUI itself that performs that process "
            "for smart contracts.")
            .Flush();
        return -1;
    }
    // ----------
    bool all = ("" == indices || "all" == indices);

    const std::int32_t nNumlistCount =
        all ? 0 : SwigWrap::NumList_Count(indices);

    // NOTE: If we are processing multiple indices, then the return value
    // is 1, since some indices may succeed and some may fail. So our return
    // value merely communicates: The processing was performed.
    //
    // ===> Whereas if there is only ONE index, then we need to set the return
    // value directly to the result of processing that index. Just watch
    // nReturnValue
    // to see how that is being done.
    //
    std::int32_t nReturnValue = 1;

    for (std::int32_t i = items - 1; 0 <= i; i--) {
        if (all || SwigWrap::NumList_VerifyQuery(indices, std::to_string(i))) {
            if (bIsDefinitelyPaymentPlan) {
                std::string instrument =
                    get_payment_instrument(transport_notary, mynym, i, inbox);
                if (instrument.empty()) {
                    LogNormal(OT_METHOD)(__FUNCTION__)(
                        ": accept_from_paymentbox: "
                        "Error: cannot get payment instrument from "
                        "inpayments box.")
                        .Flush();
                    return -1;
                }

                std::string recipient =
                    SwigWrap::Instrmnt_GetRecipientNymID(instrument);

                std::int32_t nTemp = confirm_payment_plan(
                    transport_notary,
                    mynym,
                    myacct,
                    recipient,
                    instrument,
                    i,
                    pOptionalOutput);
                if (1 == nNumlistCount) {  // If there's exactly 1 instrument
                                           // being singled-out
                    nReturnValue = nTemp;  // for processing, then return its
                                           // success/fail status.
                    break;  // Since there's only one, might as well break;
                }
            } else {
                std::int32_t nTemp = processPayment(
                    transport_notary,
                    myacct,
                    paymentType,
                    inbox,
                    i,
                    pOptionalOutput);
                if (1 == nNumlistCount) {  // If there's exactly 1 instrument
                                           // being singled-out
                    nReturnValue = nTemp;  // for processing, then return its
                                           // success/fail status.
                    break;  // Since there's only one, might as well break;
                }
            }
        }
    }

    return nReturnValue;
}

std::string RecordList::inputText(const char* what)  // static method
{
    LogNormal(OT_METHOD)(__FUNCTION__)(": Please paste ")(what)(
        "followed by an EOF or a ~ on a line by itself:")
        .Flush();

    std::string input = OT_CLI_ReadUntilEOF();
    if (input.empty()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: you did not paste ")(what)(
            ".")
            .Flush();
    }
    return input;
}

std::int32_t RecordList::processPayment(  // a static method
    const std::string& transport_notary,
    const std::string& myacct,
    const std::string& paymentType,
    const std::string& inbox,
    const std::int32_t index,
    std::string* pOptionalOutput /*=nullptr*/,
    bool CLI_input_allowed /*=false*/)
{
    if (myacct.empty()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Failure: myacct not a valid string.")
            .Flush();
        return -1;
    }

    auto accountNotaryId = Identifier::Factory();
    std::string acct_server = SwigWrap::GetAccountWallet_NotaryID(myacct);
    if (acct_server.empty()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot determine "
                                           "acct_server from myacct.")
            .Flush();
        return -1;
    }
    accountNotaryId = Identifier::Factory(acct_server);

    std::string mynym = SwigWrap::GetAccountWallet_NymID(myacct);
    if (mynym.empty()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot determine mynym from myacct.")
            .Flush();
        return -1;
    }

    std::string instrument = "";
    if (-1 == index) {
        if (CLI_input_allowed) { instrument = inputText("the instrument"); }
        if (instrument.empty()) { return -1; }
    } else {
        instrument = RecordList::get_payment_instrument(
            transport_notary, mynym, index, inbox);
        if (instrument.empty()) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: cannot get payment instrument.")
                .Flush();
            return -1;
        }
    }

    auto thePayment{Opentxs::Client().Factory().Payment(
        String::Factory(instrument.c_str()))};

    OT_ASSERT(false != bool(thePayment));

    if (!thePayment->IsValid() || !thePayment->SetTempValues()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: Failed loading payment "
                                           "instrument from string.")
            .Flush();
        return -1;
    }

    // The Notary ID we found on the payment instrument itself.
    //
    auto paymentNotaryId = Identifier::Factory();
    const bool bGotPaymentNotaryId = thePayment->GetNotaryID(paymentNotaryId);

    if (!bGotPaymentNotaryId) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: Failed getting Notary ID "
                                           "from payment instrument.")
            .Flush();
        return -1;
    }

    auto assetTypeId = Identifier::Factory();
    const bool bGotPaymentAssetTypeId =
        thePayment->GetInstrumentDefinitionID(assetTypeId);

    if (!bGotPaymentAssetTypeId) {
        // Allowed for now.

        //        otOut << "Error: cannot determine instrument's asset type.\n";
        //        return -1;
        ;
    }
    const std::string strPaymentAssetTypeId = assetTypeId->str();
    std::string type = thePayment->GetTypeString();

    std::string strIndexErrorMsg = "";
    if (-1 != index) {
        strIndexErrorMsg = "at index " + std::to_string(index) + " ";
    }

    if (!paymentType.empty() &&  // If there is a payment type specified..
        paymentType != "ANY" &&  // ...and if that type isn't "ANY"...
        paymentType != type)     // ...and it's the wrong type:
    {                            // Then skip this one.
        // Except:
        if (("CHEQUE" == paymentType || "VOUCHER" == paymentType) &&
            (thePayment->IsCheque() || thePayment->IsVoucher())) {
            // in this case we allow it to drop through.
        } else {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: invalid instrument type.")
                .Flush();
            return -1;
        }
    }

    const bool bIsPaymentPlan = thePayment->IsPaymentPlan();
    const bool bIsSmartContract = thePayment->IsSmartContract();

    if (bIsPaymentPlan) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: Cannot process a payment plan here. You HAVE to "
            "explicitly confirm it using confirmInstrument instead of "
            "processPayment.")
            .Flush();
        // NOTE: I could remove this block and it would still work. I'm just
        // deliberately disallowing payment plans here, so you are forced to
        // explicitly confirm a payment plan. Otherwise here you might confirm
        // a dozen plans under "ANY" and it's just too easy for them to slip
        // by.
        return -1;
    }

    if (bIsSmartContract) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: Cannot process a smart contract here. You HAVE to "
            "provide that functionality in your GUI directly, since you "
            "may have to choose various accounts as part of the "
            "activation process, and your user will need to probably do "
            "that in a GUI wizard. It's not so simple as in this function "
            "where you just have 'myacct'.")
            .Flush();
        return -1;
    }

    // Note: I USED to check the ASSET TYPE ID here, but then I removed it,
    // since details_deposit_cheque() already verifies that (so I don't need
    // to do it twice.)

    // By this point, we know the invoice has the right instrument definition
    // for the account we're trying to use (to pay it from.)
    //
    // But we need to make sure the invoice is made out to mynym (or to no
    // one.) Because if it IS endorsed to a Nym, and mynym is NOT that nym,
    // then the transaction will fail. So let's check, before we bother
    // sending it...

    auto senderNymId = Identifier::Factory(),
         recipientNymId = Identifier::Factory();
    std::string sender, recipient;

    if (thePayment->GetSenderNymID(senderNymId)) sender = senderNymId->str();
    if (thePayment->GetRecipientNymID(recipientNymId))
        recipient = recipientNymId->str();

    std::string endorsee = bIsPaymentPlan ? sender : recipient;

    // Not all instruments have a specified recipient. But if they do, let's
    // make sure the Nym matches.
    if (!endorsee.empty() && (endorsee != mynym)) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": The instrument ")(
            strIndexErrorMsg)(" is endorsed to a specific ")(
            bIsPaymentPlan ? "customer" : "recipient")(" (")(endorsee)(
            ") and it doesn't match the account's owner NymId (")(mynym)(
            "). This is a problem, for example, because you can't deposit "
            "a cheque into your own account, if the cheque is made out to "
            "someone else. (Skipping.)Try specifying a different "
            "account, using --myacct ACCT_ID.")
            .Flush();
        return -1;
    }

    // At this point I know the invoice isn't made out to anyone, or if it is,
    // it's properly made out to the owner of the account which I'm trying to
    // use to pay the invoice from. So let's pay it!
    // P.S. recipient might be empty, but mynym is guaranteed to be good.

    std::string accountAssetType =
        SwigWrap::GetAccountWallet_InstrumentDefinitionID(myacct);

    if (!strPaymentAssetTypeId.empty() &&
        accountAssetType != strPaymentAssetTypeId) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": The instrument at index ")(index)(
            " has a different unit type than the selected account. "
            "(Skipping). Try specifying a different account, using "
            "--myacct ACCT_ID.")
            .Flush();
        return -1;
    }
    // ---------------------------------------------
    if (paymentNotaryId != accountNotaryId) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": The instrument at index ")(index)(
            " has a different Notary ID than the selected account. "
            "(Skipping). Try specifying a different account, using "
            "--myacct ACCT_ID.")
            .Flush();
        return -1;
    }
    // Below this point we know for a fact that the notary ID on the payment
    // instrument is the same as the notary ID for the account (myacct).
    // They are interchangeable. So presumably the caller already did the
    // heavy lifting of selecting the correct account.
    // ---------------------------------------------
    //    if (bIsPaymentPlan) {
    //        // Note: this block is currently unreachable/disallowed.
    //        //       (But it would otherwise work.)
    //        //
    //        // NOTE: We couldn't even do this for smart contracts, since
    //        // the "confirmSmartContract" function assumes it's being used
    //        // at the command line, and it asks the user to enter various
    //        // data (choose your account, etc) at the command line.
    //        // So ONLY with Payment Plans can we do this here! The GUI has
    //        // to provide its own custom code for smart contracts. However,
    //        // that code will be easy to write: Just copy the code you see
    //        // in confirmInstrument, for smart contracts, and change it to
    //        // use GUI input/output instead of command line i/o.
    //        //
    //        CmdConfirm cmd;
    //        return cmd.confirmInstrument(
    //            acct_server,
    //            mynym,
    //            myacct,
    //            recipient,
    //            instrument,
    //            index,
    //            pOptionalOutput);
    //        // NOTE: we don't perform any RecordPayment here because
    //        // confirmInstrument already does that.
    //    }
    // ---------------------------------------------
    time64_t now = SwigWrap::GetTime();
    time64_t from{};
    time64_t until{};

    bool bGotValidFrom{false};
    bGotValidFrom = thePayment->GetValidFrom(from);
    bool bGotValidTo{false};
    bGotValidTo = thePayment->GetValidTo(until);

    if (!bGotValidFrom) {
        ;  // TODO Maybe log here. But I think this case is allowed.
    }
    if (!bGotValidTo) {
        ;  // TODO Maybe log here. But I think this case is allowed.
    }
    // TODO here: make sure the logic accounts for the fact that the boolean
    // could be false. We should behave properly in the event that a valid from
    // or valid to date isn't set on this particular instrument, or if it has
    // a zero value or whatever.
    //
    if (now < from) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": The instrument at index ")(index)(
            " is not yet within its valid date range. (Skipping).")
            .Flush();
        return -1;
    }

    if (until > OT_TIME_ZERO && now > until) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": The instrument at index ")(index)(
            " is expired. (Moving it to the record box).")
            .Flush();

        // Since this instrument is expired, remove it from the payments inbox,
        // and move to record box.
        if (0 <= index && SwigWrap::RecordPayment(
                              transport_notary, mynym, true, index, true)) {
            return 0;
        }

        return -1;
    }

    // IMPORTANT: After the below deposits are completed successfully, the
    // wallet will receive a "successful deposit" server reply. When that
    // happens, OT (internally) needs to go and see if the deposited item was a
    // payment in the payments inbox. If so, it should REMOVE it from that box
    // and move it to the record box.
    //
    // That's why you don't see me messing with the payments inbox even when
    // these are successful. They DO need to be removed from the payments inbox,
    // but just not here in the script. (Rather, internally by OT itself.)
    if (thePayment->IsCheque() || thePayment->IsVoucher() ||
        thePayment->IsInvoice()) {
        return depositCheque(
            acct_server, myacct, mynym, instrument, pOptionalOutput);
    } else if (thePayment->IsPurse()) {
        std::int32_t success{-1};
#if OT_CASH
        success = Opentxs::Client().Cash().deposit_purse(
            acct_server, myacct, mynym, instrument, "", pOptionalOutput);
#endif  // OT_CASH
        // if index != -1, go ahead and call RecordPayment on the purse at that
        // index, to remove it from payments inbox and move it to the recordbox.
        if (index != -1 && 1 == success) {
            SwigWrap::RecordPayment(transport_notary, mynym, true, index, true);
        }

        return success;
    }

    LogNormal(OT_METHOD)(__FUNCTION__)(
        ": Skipping this instrument: Expected CHEQUE, VOUCHER, INVOICE, "
        "or (cash) PURSE.")
        .Flush();

    return -1;
}

std::int32_t RecordList::depositCheque(  // a static method
    const std::string& server,
    const std::string& myacct,
    const std::string& mynym,
    const std::string& instrument,
    std::string* pOptionalOutput /*=nullptr*/)
{
    std::string assetType =
        SwigWrap::GetAccountWallet_InstrumentDefinitionID(myacct);
    if (assetType.empty()) { return -1; }

    if (assetType != SwigWrap::Instrmnt_GetInstrumentDefinitionID(instrument)) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: instrument definitions of instrument and myacct do "
            "not match.")
            .Flush();
        return -1;
    }

    auto cheque{Opentxs::Client().Factory().Cheque()};

    OT_ASSERT(false != bool(cheque));

    cheque->LoadContractFromString(String::Factory(instrument.c_str()));

    std::string response;
    {
        response = Opentxs::Client()
                       .ServerAction()
                       .DepositCheque(
                           Identifier::Factory(mynym),
                           Identifier::Factory(server),
                           Identifier::Factory(myacct),
                           cheque)
                       ->Run();
    }
    std::int32_t reply = InterpretTransactionMsgReply(
        Opentxs::Client(), server, mynym, myacct, "deposit_cheque", response);
    if (1 != reply) { return reply; }

    if (nullptr != pOptionalOutput) { *pOptionalOutput = response; }

    {
        if (!Opentxs::Client().ServerAction().DownloadAccount(
                Identifier::Factory(mynym),
                Identifier::Factory(server),
                Identifier::Factory(myacct),
                true)) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error retrieving intermediary files for account.")
                .Flush();
            return -1;
        }
    }
    return 1;
}

std::int32_t RecordList::confirm_payment_plan(  // static method
    const std::string& server,
    const std::string& mynym,
    const std::string& myacct,
    const std::string& hisnym,
    const std::string& instrument,
    std::int32_t index,
    std::string* pOptionalOutput /*=nullptr*/)
{
    std::string instrumentType = SwigWrap::Instrmnt_GetType(instrument);
    if (instrumentType.empty() ||
        (0 != instrumentType.compare("PAYMENT PLAN"))) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: instrument is empty, or is not a payment plan.")
            .Flush();
        return -1;
    }

    time64_t now = SwigWrap::GetTime();
    time64_t from = SwigWrap::Instrmnt_GetValidFrom(instrument);
    if (now < from) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": The instrument is not yet valid.")
            .Flush();
        return 0;
    }

    time64_t until = SwigWrap::Instrmnt_GetValidTo(instrument);
    if (until > OT_TIME_ZERO && now > until) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": The instrument has already expired.")
            .Flush();

        if (-1 == index) {
            if (!SwigWrap::Msg_HarvestTransactionNumbers(
                    instrument, mynym, false, false, false, false, false)) {
                return -1;
            }
            return 0;
        }

        // Since this instrument is expired, remove it from the payments inbox,
        // and move it to record box.
        if (!SwigWrap::RecordPayment(server, mynym, true, index, true)) {
            return -1;
        }
        return 0;
    }

    return confirmPaymentPlan_lowLevel(
        mynym, myacct, instrument, pOptionalOutput);
}

std::int32_t RecordList::confirmPaymentPlan_lowLevel(  // a static method
    const std::string& mynym,
    const std::string& myacct,
    const std::string& plan,
    std::string* pOptionalOutput /*=nullptr*/)
{
    // Very possibly, the server where we RECEIVED the payment plan
    // is not necessarily the server where it's DRAWN on. Therefore
    // we must read the server ID directly from the instrument, and
    // not use the server ID of the payments inbox where this instrument
    // came from.
    //
    std::string server = SwigWrap::Instrmnt_GetNotaryID(plan);
    if (server.empty()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot get server from instrument.")
            .Flush();
        return -1;
    }

    std::string senderUser = mynym;

    if (senderUser.empty()) {
        senderUser = SwigWrap::Instrmnt_GetSenderNymID(plan);
    }

    if (senderUser.empty()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot get sender user from instrument.")
            .Flush();
        return -1;
    }

    std::string senderAcct = myacct;

    if (senderAcct.empty()) {
        senderAcct = SwigWrap::Instrmnt_GetSenderAcctID(plan);
    }

    if (senderAcct.empty()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot get sender account from instrument.")
            .Flush();
        return -1;
    }

    std::string recipientUser = SwigWrap::Instrmnt_GetRecipientNymID(plan);
    if (recipientUser.empty()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot get recipient user from instrument.")
            .Flush();
        return -1;
    }

    std::string recipientAcct = SwigWrap::Instrmnt_GetRecipientAcctID(plan);
    if (recipientAcct.empty()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot get recipient account from instrument.")
            .Flush();
        return -1;
    }

    {
        if (!Opentxs::Client().ServerAction().GetTransactionNumbers(
                Identifier::Factory(senderUser),
                Identifier::Factory(server),
                2)) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: cannot reserve transaction numbers.")
                .Flush();
            return -1;
        }
    }

    std::string confirmed = SwigWrap::ConfirmPaymentPlan(
        server, senderUser, senderAcct, recipientUser, plan);
    if (confirmed.empty()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot confirm payment plan.")
            .Flush();
        return -1;
    }

    // NOTE: If we fail, then we need to harvest the transaction numbers
    // back from the payment plan that we confirmed.
    auto paymentPlan{Opentxs::Client().Factory().PaymentPlan()};

    OT_ASSERT(false != bool(paymentPlan));

    paymentPlan->LoadContractFromString(String::Factory(confirmed.c_str()));

    std::string response;
    {
        response = Opentxs::Client()
                       .ServerAction()
                       .DepositPaymentPlan(
                           Identifier::Factory(senderUser),
                           Identifier::Factory(server),
                           paymentPlan)
                       ->Run();
    }

    std::int32_t success = VerifyMessageSuccess(Opentxs::Client(), response);
    if (1 != success) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot deposit payment plan.")
            .Flush();
        SwigWrap::Msg_HarvestTransactionNumbers(
            confirmed, senderUser, false, false, false, false, false);
        return success;
    }

    std::int32_t reply = InterpretTransactionMsgReply(

        Opentxs::Client(),
        server,
        senderUser,
        senderAcct,
        "deposit_payment_plan",
        response);

    if (1 != reply) { return reply; }

    if (nullptr != pOptionalOutput) { *pOptionalOutput = response; }

    {
        if (!Opentxs::Client().ServerAction().DownloadAccount(
                Identifier::Factory(senderUser),
                Identifier::Factory(server),
                Identifier::Factory(senderAcct),
                true)) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error retrieving intermediary "
                "files for account.")
                .Flush();
            return -1;
        }
    }

    // NOTICE here (on success) we do NOT call RecordPayment. (Contrast this
    // with confirmSmartContract, below.) Why does it call RecordPayment for a
    // smart contract, yet here we do not?
    //
    // Because with a smart contract, it has been sent on to the next party. So
    // it's now got a copy in the outpayments.
    // There was no transaction performed at that time; perhaps it will go to
    // 3 more parties before it's actually activated onto a server. Only THEN
    // is a transaction performed. Therefore, confirmSmartContract is finished
    // with the incoming instrument, in every respect, and can directly
    // RecordPayment it to remove it from the payments inbox.
    //
    // Whereas with a payment plan, a transaction HAS just been performed.
    // (See the call just above, to deposit_payment_plan.) Therefore, OT has
    // already received the server reply at a lower level, and OT already should
    // be smart enough to RecordPayment at THAT time.
    //
    // And that's why we don't need to do it here and now: Because it has
    // already been done.

    return 1;
}

// static
bool RecordList::checkMandatory(const char* name, const std::string& value)
{
    if (value.empty()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: ")(name)(
            ": mandatory parameter not specified.")
            .Flush();
    }

    return true;
}

// static
bool RecordList::checkIndices(const char* name, const std::string& indices)
{
    if (!checkMandatory(name, indices)) { return false; }

    if ("all" == indices) { return true; }

    for (std::string::size_type i = 0; i < indices.length(); i++) {
        if (!isdigit(indices[i])) {
            LogNormal(OT_METHOD)(__FUNCTION__)(": Error: ")(name)(
	       ": not a value: ")(indices)(".")
                .Flush();
            return false;
        }
        for (i++; i < indices.length() && isdigit(indices[i]); i++) {}
        if (i < indices.length() && ',' != indices[i]) {
            LogNormal(OT_METHOD)(__FUNCTION__)(": Error: ")(name)(
	       ": not a value: ")(indices)(".")
                .Flush();
            return false;
        }
    }

    return true;
}

// static
bool RecordList::checkServer(const char* name, std::string& server)
{
    if (!checkMandatory(name, server)) return false;

    auto theID = Identifier::Factory(server);
    ConstServerContract pServer;  // shared_ptr to const.
    const auto& wallet = Opentxs::Client().Wallet();

    // See if it's available using the full length ID.
    if (!theID->empty()) pServer = wallet.Server(theID);

    if (!pServer) {
        const auto servers = wallet.ServerList();

        // See if it's available using the partial length ID.
        for (auto& it : servers) {
            if (0 == it.first.compare(0, server.length(), server)) {
                pServer = wallet.Server(Identifier::Factory(it.first));
                break;
            }
        }
        if (!pServer) {
            // See if it's available using the full length name.
            for (auto& it : servers) {
                if (0 == it.second.compare(0, it.second.length(), server)) {
                    pServer = wallet.Server(Identifier::Factory(it.first));
                    break;
                }
            }

            if (!pServer) {
                // See if it's available using the partial name.
                for (auto& it : servers) {
                    if (0 == it.second.compare(0, server.length(), server)) {
                        pServer = wallet.Server(Identifier::Factory(it.first));
                        break;
                    }
                }
            }
        }
    }

    if (!pServer) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: ")(name)(
	   ": unknown server: ")(server)(".")
            .Flush();
        return false;
    }

    server = pServer->ID()->str();
    LogNormal(OT_METHOD)(__FUNCTION__)(": Using ")
      (name)(": ")(server)(".").Flush();
    return true;
}

// static
bool RecordList::checkNym(
    const char* name,
    std::string& nym,
    bool checkExistance /*=false*/)
{
    if (!checkMandatory(name, nym)) return false;

    ConstNym pNym = nullptr;
    const auto nymID = Identifier::Factory(nym);
    const auto& wallet = Opentxs::Client().Wallet();

    if (!nymID->empty()) pNym = wallet.Nym(nymID);

    if (!pNym) pNym = wallet.NymByIDPartialMatch(nym);

    if (pNym) {
        auto tmp = String::Factory();
        pNym->GetIdentifier(tmp);
        nym = tmp->Get();
    } else if (checkExistance) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: ")(name)(
	   ": unknown nym: ")(nym)(".")
            .Flush();
        return false;
    }

    LogNormal(OT_METHOD)(__FUNCTION__)(
       ": Using ")(name)(": ")(nym)(".").Flush();
    return true;
}

// static
bool RecordList::checkAccount(const char* name, std::string& accountID)
{
    if (!checkMandatory(name, accountID)) { return false; }

    auto theID = Identifier::Factory(accountID);

    if (theID->empty()) { return false; }

    auto account = Opentxs::Client().Wallet().Account(theID);

    if (false == bool(account)) {
        const auto converted =
            Opentxs::Client().Wallet().AccountPartialMatch(accountID);

        if (converted->empty()) {
            LogNormal(OT_METHOD)(__FUNCTION__)(": Error: ")(name)(
	       ": unknown account: ")(accountID)(".")
                .Flush();
            return false;
        }

        account = Opentxs::Client().Wallet().Account(converted);
    }

    OT_ASSERT(account)

    accountID = account.get().GetPurportedAccountID().str();
    LogDetail(OT_METHOD)(__FUNCTION__)(
       ": Using ")(name)(": ")(accountID)(".")
        .Flush();

    return true;
}

// static
std::int32_t RecordList::discard_incoming_payments(
    const std::string& transportNotaryId,
    const std::string& mynym,
    const std::string& indices)
{
    std::string the_server = transportNotaryId;
    if (!checkServer("server", the_server)) { return -1; }

    std::string the_mynym = mynym;
    if (!checkNym("mynym", the_mynym)) { return -1; }

    if (!checkIndices("indices", indices)) { return -1; }

    std::string inbox = SwigWrap::LoadPaymentInbox(the_server, the_mynym);
    if (inbox.empty()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot load payment inbox.")
            .Flush();
        return -1;
    }

    std::int32_t items =
        SwigWrap::Ledger_GetCount(the_server, the_mynym, the_mynym, inbox);
    if (0 > items) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot load payment inbox item count.")
            .Flush();
        return -1;
    }

    if (0 == items) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": The payment inbox is empty.")
            .Flush();
        return 0;
    }

    bool all = ("all" == indices);

    // Loop from back to front, in case any are removed.
    std::int32_t retVal = 1;
    for (std::int32_t i = items - 1; 0 <= i; i--) {
        if (!all &&
            !SwigWrap::NumList_VerifyQuery(indices, std::to_string(i))) {
            continue;
        }

        if (!SwigWrap::RecordPayment(the_server, the_mynym, true, i, false)) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: cannot discard payment.")
                .Flush();
            retVal = -1;
            continue;
        }

        LogNormal(OT_METHOD)(__FUNCTION__)(": Success discarding payment!")
            .Flush();
    }

    return retVal;
}

// static
std::int32_t RecordList::cancel_outgoing_payments(
    const std::string& mynym,
    const std::string& myacct,
    const std::string& indices)
{
    // NOTE: You can't just discard a sent cheque  from your outpayment box.
    // Why not? Just because you remove your record of the outgoing cheque,
    // doesn't mean you didn't already send it. (The recipient still received
    // it, and still has it, whether you remove it from your outbox or not.)
    // If you really want to cancel the cheque, then you need to do it in such
    // a way that it will fail-as-cancelled when the recipient tries to deposit
    // it. Otherwise, he would get that money whether you kept your own outgoing
    // record or not. Therefore SOME server message must be performed here,
    // which actually cancels the transaction number itself that appears on the
    // cheque. This is the only way to insure that the cheque can't be used by
    // the recipient (and even this will only work if you beat him to the punch.
    // If he deposits it before you cancel it, then it's already too late and he
    // has the money.) THIS is why RecordPayment, regarding outpayments, only
    // works on expired instruments -- because if it's not expired, you don't
    // just want to remove your record of it. You want to cancel the transaction
    // number itself -- and that requires server communication.

    std::string the_mynym = mynym;
    if (!checkNym("mynym", the_mynym)) { return -1; }

    std::string the_myacct = myacct;
    if (!myacct.empty() && !checkAccount("myacct", the_myacct)) { return -1; }

    if (!checkIndices("indices", indices)) { return -1; }

    std::int32_t items = SwigWrap::GetNym_OutpaymentsCount(the_mynym);
    if (0 > items) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot load payment outbox item count.")
            .Flush();
        return -1;
    }

    if (0 == items) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": The payment outbox is empty.")
            .Flush();
        return 0;
    }

    bool all = ("all" == indices);

    // Loop from back to front, in case any are removed.
    std::int32_t retVal = 1;
    for (std::int32_t i = items - 1; 0 <= i; i--) {
        if (!all &&
            !SwigWrap::NumList_VerifyQuery(indices, std::to_string(i))) {
            continue;
        }

        std::string payment =
            SwigWrap::GetNym_OutpaymentsContentsByIndex(the_mynym, i);
        if (payment.empty()) {
            LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot load payment ")(
                i)(".")
                .Flush();
            retVal = -1;
            continue;
        }

        std::string server =
            SwigWrap::GetNym_OutpaymentsNotaryIDByIndex(the_mynym, i);
        if (server.empty()) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: cannot load server for payment ")(i)(".")
                .Flush();
            retVal = -1;
            continue;
        }

        // Done: Put the code here where we message the server to cancel all
        // relevant transaction numbers for the instrument. If it's a cheque,
        // there's only one number. But if it's a smart contract, there could
        // be many numbers. Seems like best thing is to just activate it, but
        // have a "rejected" flag which causes the activation to fail. (That
        // way, all the other parties will be properly notified, which the
        // server already does.) We don't even need to remove it from the
        // outpayment box, because the failure notification from the server
        // will already cause the OTClient to remove it from the outpayment box.
        //
        // Ah-ha! ANY outgoing payment plan or smart contract is necessarily
        // incomplete: it's outgoing because it was sent to the next party so
        // he could sign it, too, and probably activate it. Presumably he has
        // not done so yet (if I am 'beating him to the punch' by cancelling it
        // before he can activate it) and therefore the plan or smart contract
        // still is missing at least one signer, so it is GUARANTEED to fail
        // verification if I try to activate it myself. (Good.)
        //
        // This means I can just take whatever instrument appears outgoing,
        // and try to activate it myself. It will definitely fail activation,
        // and then the failure notice will already be sent from that, to all
        // the parties, and they can harvest back their numbers automatically
        // as necessary.
        //
        // The one problem is, though this works for payment plans and smart
        // contracts, it will not work for cheques. The cheque is made out to
        // someone else, and he is the one normally who needs to deposit it.
        // Plus, I can't deposit a cheque into the same account it's drawn on.
        //
        // UPDATE: Now when a cheque is deposited into the same account it was
        // drawn on, that will be interpreted by the server as a request to
        // CANCEL the cheque.

        const auto theNotaryID = Identifier::Factory(server),
                   theNymID = Identifier::Factory(the_mynym);
        std::string type = SwigWrap::Instrmnt_GetType(payment);

        if ("SMARTCONTRACT" == type) {
            // Just take the smart contract from the outpayment box, and try to
            // activate it. It WILL fail, and then the failure message will be
            // propagated to all the other parties to the contract. (Which will
            // result in its automatic removal from the outpayment box.)

            // FIX: take myacct from smart contract instead of --myacct
            if (the_myacct.empty()) {
                LogNormal(OT_METHOD)(__FUNCTION__)(
                    ": You MUST provide --myacct for smart contracts.")
                    .Flush();
                retVal = -1;
                continue;
            }

            // Try to activate the smart contract. (As a way of  cancelling it.)
            // So while we expect this 'activation' to fail, it should have the
            // desired effect of cancelling the smart contract and sending
            // failure notices to all the parties.
            auto contract{Opentxs::Client().Factory().SmartContract()};

            OT_ASSERT(false != bool(contract));

            contract->LoadContractFromString(String::Factory(payment));
            std::string response;
            {
                response = Opentxs::Client()
                               .ServerAction()
                               .ActivateSmartContract(
                                   theNymID,
                                   theNotaryID,
                                   Identifier::Factory(the_myacct),
                                   "acct_agent_name",
                                   contract)
                               ->Run();
            }
            if (response.empty()) {
                LogNormal(OT_METHOD)(__FUNCTION__)(
                    ": Error: cannot cancel smart contract.")
                    .Flush();
                retVal = -1;
                continue;
            }

            LogNormal(OT_METHOD)(__FUNCTION__)(": Server Reply: ").Flush();
            LogNormal(OT_METHOD)(__FUNCTION__)(response).Flush();

            if (1 != SwigWrap::Message_IsTransactionCanceled(
                         server, the_mynym, the_myacct, response)) {
                LogNormal(OT_METHOD)(__FUNCTION__)(
                    ": Error: cancel smart contract failed.")
                    .Flush();
                retVal = -1;
                continue;
            }

            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Success canceling smart contract!")
                .Flush();
            continue;
        }

        if ("PAYMENT PLAN" == type) {
            // Just take the payment plan from the outpayment box, and try to
            // activate it. It WILL fail, and then the failure message will be
            // propagated to the other party to the contract. (Which will result
            // in its automatic removal from the outpayment box.)

            auto plan{Opentxs::Client().Factory().PaymentPlan()};

            OT_ASSERT(false != bool(plan));

            plan->LoadContractFromString(String::Factory(payment));
            std::string response;
            {
                response = Opentxs::Client()
                               .ServerAction()
                               .CancelPaymentPlan(theNymID, theNotaryID, plan)
                               ->Run();
            }
            if (response.empty()) {
                LogNormal(OT_METHOD)(__FUNCTION__)(
                    ": Error: cannot cancel payment plan.")
                    .Flush();
                retVal = -1;
                continue;
            }

            LogNormal(OT_METHOD)(__FUNCTION__)
	       (": Server reply: ")(response)(".")
                .Flush();

            if (1 != SwigWrap::Message_IsTransactionCanceled(
                         server, the_mynym, the_myacct, response)) {
                LogNormal(OT_METHOD)(__FUNCTION__)(
                    ": Error: cancel payment plan failed.")
                    .Flush();
                retVal = -1;
                continue;
            }

            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Success canceling payment plan!")
                .Flush();
            continue;
        }

        if ("PURSE" == type) {
            // This is a tricky one -- why would anyone EVER want to discard
            // outgoing cash? Normally your incentive would be to do the
            // opposite: Keep a copy of all outgoing cash until the copy
            // itself expires (when the cash expires.) This way it's always
            // recoverable in the event of a "worst case" situation.
            //
            // So what do we do in this case? Nevertheless, the user has
            // explicitly just instructed the client to DISCARD OUTGOING CASH.
            //
            // Perhaps we should just ask the user to CONFIRM that he wants to
            // erase the cash, and make SURE that he understands the
            // consequences of that choice.

            // removes payment instrument (from payments in or out box)
            if (!SwigWrap::RecordPayment(server, the_mynym, false, i, false)) {
                LogNormal(OT_METHOD)(__FUNCTION__)(
                    ": Error: cannot cancel cash purse.")
                    .Flush();
                retVal = -1;
                continue;
            }

            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Success canceling cash purse!")
                .Flush();
            continue;
        }

        // CHEQUE VOUCHER INVOICE

        bool isVoucher = ("VOUCHER" == type);

        // Get the nym and account IDs from the cheque itself.
        std::string acctID = isVoucher
                                 ? SwigWrap::Instrmnt_GetRemitterAcctID(payment)
                                 : SwigWrap::Instrmnt_GetSenderAcctID(payment);
        if (acctID.empty()) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: cannot retrieve asset account ID.")
                .Flush();
            retVal = -1;
            continue;
        }

        std::string nymID = isVoucher
                                ? SwigWrap::Instrmnt_GetRemitterNymID(payment)
                                : SwigWrap::Instrmnt_GetSenderNymID(payment);
        if (nymID.empty()) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: cannot retrieve sender nym.")
                .Flush();
            retVal = -1;
            continue;
        }

        if (nymID != the_mynym) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: unexpected sender nym.")
                .Flush();
            retVal = -1;
            continue;
        }

        if (1 != depositCheque(server, acctID, nymID, payment)) {
            LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot cancel ")(type)(
                ".")
                .Flush();
            retVal = -1;
            continue;
        }

        LogNormal(OT_METHOD)(__FUNCTION__)(": Success canceling ")(type)("!")
            .Flush();
    }
    return retVal;
}

std::int32_t RecordList::acceptFromInbox(  // a static method
    const std::string& myacct,
    const std::string& indices,
    const std::int32_t itemTypeFilter)
{
    std::string server = SwigWrap::GetAccountWallet_NotaryID(myacct);
    if (server.empty()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot determine server from myacct.")
            .Flush();
        return -1;
    }

    std::string mynym = SwigWrap::GetAccountWallet_NymID(myacct);
    if (mynym.empty()) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot determine mynym from myacct.")
            .Flush();
        return -1;
    }
    // -----------------------------------------------------------
    // NOTE: I just removed this during my most recent changes. It just seems,
    // now with Justus' regular automated refreshes, I shouldn't have to grab
    // these right BEFORE, when I then anyway have to grab them right AFTER
    // (at the end of this function) once it succeeds. Should speed things
    // up?
    // Also, I considered the fact that we're using indices in this function
    // So if the user has actually selected certain indices already, then seems
    // unwise to download the inbox before processing THOSE indices. Rather
    // have it fail and re-try, and at least be trying the actual intended
    // indices, and cut our account retrievals in half while we're at it!
    //
    //    if (!Opentxs::Client().ME().retrieve_account(server, mynym, myacct,
    //    true)) {
    //        otOut << "Error retrieving intermediary files for account.\n";
    //        return -1;
    //    }
    // -----------------------------------------------------------
    // NOTE: Normally we don't have to do this, because the high-level API is
    // smart enough, when sending server transaction requests, to grab new
    // transaction numbers if it is running low. But in this case, we need the
    // numbers available BEFORE sending the transaction request, because the
    // call to SwigWrap::Ledger_CreateResponse is where the number is first
    // needed, and that call is made before the server transaction request is
    // actually sent.
    //
    const auto theNotaryID = Identifier::Factory(server),
               theNymID = Identifier::Factory(mynym),
               theAcctID = Identifier::Factory(myacct);

    {
        if (!Opentxs::Client().ServerAction().GetTransactionNumbers(
                theNymID, theNotaryID, 10)) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: cannot reserve transaction numbers.")
                .Flush();
            return -1;
        }
    }
    // -----------------------------------------------------------
    auto pInbox(
        Opentxs::Client().OTAPI().LoadInbox(theNotaryID, theNymID, theAcctID));
    if (false == bool(pInbox)) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot load inbox.")
            .Flush();
        return -1;
    }
    // -----------------------------------------------------------
    std::int32_t item_count = pInbox->GetTransactionCount();
    // -----------------------------------------------------------
    if (0 > item_count) {
        otErr << "Error: cannot load inbox item count.\n";
        return -1;
    } else if (0 == item_count) {
        LogDetail(OT_METHOD)(__FUNCTION__)(": The inbox is empty.").Flush();
        return 0;
    }

    if (!RecordList::checkIndicesRange("indices", indices, item_count)) {
        return -1;
    }

    bool all = ("" == indices || "all" == indices);
    // -----------------------------------------------------------
    std::set<std::int32_t>* pOnlyForIndices{nullptr};
    std::set<std::int32_t> setForIndices;
    if (!all) {
        NumList numlistForIndices{indices};
        std::set<std::int64_t> setForIndices64;
        if (numlistForIndices.Output(setForIndices64)) {
            pOnlyForIndices = &setForIndices;
            for (const std::int64_t& lIndex : setForIndices64) {
                setForIndices.insert(static_cast<std::int32_t>(lIndex));
            }
        }
    }
    std::set<TransactionNumber> receiptIds{
        pInbox->GetTransactionNums(pOnlyForIndices)};

    if (receiptIds.size() < 1) {
        LogDetail(OT_METHOD)(__FUNCTION__)(
            ": There are no inbox receipts to process.")
            .Flush();
        return 0;
    }
    // -----------------------------------------------------------
    // NOTE: Indices are only optional. Otherwise it's "accept all receipts".
    // But even if you DID pass in a comma-separated list of indices, it doesn't
    // matter below this point.
    // That's because we have now translated the user-selected GUI indices into
    // an actual set of receipt IDs, each being the trans num on a transaction
    // inside the inbox.

    // -------------------------------------------------------
    OT_API::ProcessInbox response{
        Opentxs::Client().OTAPI().Ledger_CreateResponse(
            theNotaryID, theNymID, theAcctID)};
    // -------------------------------------------------------
    auto& processInbox = std::get<0>(response);
    auto& inbox = std::get<1>(response);

    if (!bool(processInbox) || !bool(inbox)) {
        LogDetail(OT_METHOD)(__FUNCTION__)(
            ": Ledger_CreateResponse somehow failed.")
            .Flush();
        return -1;
    }
    // -------------------------------------------------------
    for (const TransactionNumber& lReceiptId : receiptIds) {
        OTTransaction* pReceipt =
            Opentxs::Client().OTAPI().Ledger_GetTransactionByID(
                *inbox, lReceiptId);

        if (nullptr == pReceipt) {
            otErr << __FUNCTION__
                  << "Unexpectedly got a nullptr for ReceiptId: " << lReceiptId;
            return -1;
        }  // Below this point, pReceipt is a good pointer. It's
        //   owned by inbox, so no need to delete.
        // ------------------------
        // itemTypeFilter == 0 for all, 1 for transfers only, 2 for receipts
        // only.
        //
        if (0 != itemTypeFilter) {
            const transactionType receipt_type{pReceipt->GetType()};

            const bool transfer = (transactionType::pending == receipt_type);

            if ((1 == itemTypeFilter) && !transfer) {
                // not a pending transfer.
                continue;
            }
            if ((2 == itemTypeFilter) && transfer) {
                // not a receipt.
                continue;
            }
        }
        // ------------------------
        const bool bReceiptResponseCreated =
            Opentxs::Client().OTAPI().Transaction_CreateResponse(
                theNotaryID,
                theNymID,
                theAcctID,
                *processInbox,
                *pReceipt,
                true);

        if (!bReceiptResponseCreated) {
            otErr << __FUNCTION__
                  << "Error: cannot create transaction response.\n";
            return -1;
        }
    }  // for
    // -------------------------------------------------------
    if (processInbox->GetTransactionCount() <= 0) {
        // did not process anything
        otErr << __FUNCTION__
              << "Should never happen. Might want to follow up if you see this "
                 "log.\n";
        return 0;
    }
    // ----------------------------------------------
    const bool bFinalized = Opentxs::Client().OTAPI().Ledger_FinalizeResponse(
        theNotaryID, theNymID, theAcctID, *processInbox);

    if (!bFinalized) {
        otErr << __FUNCTION__ << "Error: cannot finalize response.\n";
        return -1;
    }
    // ----------------------------------------------
    std::string notary_response;
    {
        notary_response =
            Opentxs::Client()
                .ServerAction()
                .ProcessInbox(theNymID, theNotaryID, theAcctID, processInbox)
                ->Run();
    }
    std::int32_t reply = InterpretTransactionMsgReply(

        Opentxs::Client(),
        server,
        mynym,
        myacct,
        "process_inbox",
        notary_response);

    if (1 != reply) { return reply; }

    // We KNOW they all just changed, since we just processed
    // the inbox. Might as well refresh our copy with the new changes.
    //
    {
        if (!Opentxs::Client().ServerAction().DownloadAccount(
                theNymID, theNotaryID, theAcctID, true)) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Success processing inbox, but then failed "
                "retrieving intermediary files for account.")
                .Flush();
            //          return -1;
            // By this point we DID successfully process the inbox.
            // (We just then subsequently failed to download the updated acct
            // files.)
        }
    }

    return 1;
}

void RecordList::AddAccountID(std::string str_id)
{
    m_accounts.insert(m_accounts.end(), str_id);
}

void RecordList::ClearAccounts()
{
    ClearContents();
    m_accounts.clear();
}

void RecordList::AcceptChequesAutomatically(bool bVal)
{
    m_bAutoAcceptCheques = bVal;
}
void RecordList::AcceptReceiptsAutomatically(bool bVal)
{
    m_bAutoAcceptReceipts = bVal;
}
void RecordList::AcceptTransfersAutomatically(bool bVal)
{
    m_bAutoAcceptTransfers = bVal;
}
void RecordList::AcceptCashAutomatically(bool bVal)
{
    m_bAutoAcceptCash = bVal;
}

bool RecordList::DoesAcceptChequesAutomatically() const
{
    return m_bAutoAcceptCheques;
}
bool RecordList::DoesAcceptReceiptsAutomatically() const
{
    return m_bAutoAcceptReceipts;
}
bool RecordList::DoesAcceptTransfersAutomatically() const
{
    return m_bAutoAcceptTransfers;
}
bool RecordList::DoesAcceptCashAutomatically() const
{
    return m_bAutoAcceptCash;
}

void RecordList::notifyOfSuccessfulNotarization(
    const std::string&,
    const std::string,
    const std::string,
    const std::string,
    const std::string,
    TransactionNumber,
    TransactionNumber) const
{
}

typedef std::map<std::int32_t, std::shared_ptr<OTPayment>> mapOfPayments;

bool RecordList::PerformAutoAccept()
{
    // LOOP NYMS
    //
    if (m_bAutoAcceptCheques || m_bAutoAcceptCash) {
        std::int32_t nNymIndex = -1;
        for (auto& it_nym : m_nyms) {
            ++nNymIndex;

            if (0 == nNymIndex) {
                LogVerbose("======================================").Flush();
                LogVerbose(__FUNCTION__)(
                    ": Beginning auto-accept loop through Nyms...")
                    .Flush();
            }

            const std::string& str_nym_id(it_nym);
            const auto theNymID = Identifier::Factory(str_nym_id);
            const auto strNymID = String::Factory(theNymID);
            ConstNym pNym = wallet_.Nym(theNymID);
            if (!pNym) continue;
            // LOOP SERVERS
            //
            // For each nym, for each server, loop through its payments inbox
            //
            std::int32_t nServerIndex = -1;
            for (auto& it_server : m_servers) {
                ++nServerIndex;
                const std::string& str_msg_notary_id(it_server);
                const auto theMsgNotaryID =
                    Identifier::Factory(str_msg_notary_id);
                auto pMsgServer = wallet_.Server(theMsgNotaryID);

                if (!pMsgServer) {
                    // This can happen if the user erases the server contract
                    // from the wallet. Therefore we just need to skip it.
                    LogVerbose(__FUNCTION__)(": Skipping a notary server (")(
                        str_msg_notary_id)(
                        ") since the contract has disappeared from "
                        "the wallet. (Probably deleted by the user).")
                        .Flush();
                    continue;
                }

                const auto strMsgNotaryID = String::Factory(theMsgNotaryID);
                LogVerbose(__FUNCTION__)(": Msg Notary ")(nServerIndex)(
		   ", ID: ")(strMsgNotaryID)(".")
                    .Flush();
                mapOfPayments thePaymentMap;
                std::map<std::int32_t, TransactionNumber> mapPaymentBoxTransNum;

                // OPTIMIZE FYI:
                // The "NoVerify" version is much faster, but you will lose the
                // ability to get the
                // sender/recipient name from the receipts in the box. The code
                // will, however, work
                // either way.
                std::shared_ptr<Ledger> pInbox{nullptr};

                if (false == theNymID->empty()) {
                    pInbox = m_bRunFast
                                 ? Opentxs::Client()
                                       .OTAPI()
                                       .LoadPaymentInboxNoVerify(
                                           theMsgNotaryID, theNymID)
                                 : Opentxs::Client().OTAPI().LoadPaymentInbox(
                                       theMsgNotaryID, theNymID);
                }

                // It loaded up, so let's loop through it.
                if (false != bool(pInbox)) {
                    std::int32_t nIndex{-1};
                    for (auto& it : pInbox->GetTransactionMap()) {
                        TransactionNumber lPaymentBoxTransNum = it.first;
                        auto pBoxTrans = it.second;
                        OT_ASSERT(false != bool(pBoxTrans));
                        ++nIndex;  // 0 on first iteration.
                        //                      otInfo << __FUNCTION__
                        //                             << ": Incoming payment: "
                        //                             << nIndex << "\n";
                        const std::string* p_str_asset_type =
                            &RecordList::s_blank;  // <========== ASSET TYPE
                        const std::string* p_str_asset_name =
                            &RecordList::s_blank;  // instrument definition
                                                   // display name.
                        std::string str_type;      // Instrument type.
                        auto pPayment = GetInstrumentByReceiptID(
                            *pNym, lPaymentBoxTransNum, *pInbox);
                        if (false == bool(pPayment))  // then we treat it like
                                                      // it's abbreviated.
                        {
                            otErr << __FUNCTION__
                                  << ": Payment retrieved from payments "
                                     "inbox was nullptr. (It's abbreviated?) "
                                     "Skipping.\n";
                        }
                        // We have pPayment, the instrument accompanying the
                        // receipt in the payments inbox.
                        //
                        else if (
                            pPayment->IsValid() && pPayment->SetTempValues()) {
                            auto theInstrumentDefinitionID =
                                Identifier::Factory();

                            if (pPayment->GetInstrumentDefinitionID(
                                    theInstrumentDefinitionID)) {
                                auto strTemp =
                                    String::Factory(theInstrumentDefinitionID);
                                const std::string str_inpmt_asset(
                                    strTemp->Get());  // The instrument
                                                      // definition
                                                      // we found
                                                      // on the payment (if we
                                                      // found anything.)
                                auto it_asset = m_assets.find(str_inpmt_asset);
                                if (it_asset != m_assets.end())  // Found it on
                                                                 // the map of
                                // instrument definitions
                                // we care about.
                                {
                                    p_str_asset_type =
                                        &(it_asset->first);  // Set the asset
                                                             // type ID.
                                    p_str_asset_name = &(
                                        it_asset->second);  // The CurrencyTLA.
                                                            // Examples: USD,
                                                            // BTC, etc.
                                } else {
                                    // There was definitely an instrument
                                    // definition on the
                                    // instrument, and it definitely
                                    // did not match any of the assets that we
                                    // care about.
                                    // Therefore, skip.
                                    //
                                    otErr << __FUNCTION__
                                          << ": Skipping: Incoming payment (we "
                                             "don't care about asset "
                                          << str_inpmt_asset.c_str() << ")\n";
                                    continue;
                                }
                            }
                            // By this point, p_str_asset_type and
                            // p_str_asset_name are definitely set.
                            OT_ASSERT(nullptr != p_str_asset_type);  // and it's
                                                                     // either
                            // blank, or
                            // it's one of
                            // the asset
                            // types we
                            // care about.
                            OT_ASSERT(nullptr != p_str_asset_name);  // and it's
                                                                     // either
                            // blank, or
                            // it's one of
                            // the asset
                            // types we
                            // care about.
                            // Instrument type (cheque, voucher, etc)
                            std::int32_t nType =
                                static_cast<std::int32_t>(pPayment->GetType());

                            str_type = GetTypeString(nType);
                            // For now, we only accept cash, cheques and
                            // vouchers.
                            //
                            if ((m_bAutoAcceptCheques &&
                                 ((0 == str_type.compare("cheque")) ||
                                  (0 == str_type.compare("voucher")))) ||
                                (m_bAutoAcceptCash &&
                                 (0 == str_type.compare("cash")))) {
                                LogVerbose(__FUNCTION__)(
                                    ": Adding to acceptance list: pending "
                                    "incoming ")(str_type)(".")
                                    .Flush();

                                mapPaymentBoxTransNum.insert(
                                    std::pair<std::int32_t, TransactionNumber>(
                                        nIndex, lPaymentBoxTransNum));

                                thePaymentMap.insert(
                                    std::pair<
                                        std::int32_t,
                                        std::shared_ptr<OTPayment>>(
                                        nIndex, pPayment));
                            } else {
                                LogVerbose(__FUNCTION__)(
                                    ": Instrument type not enabled for "
                                    "auto-accept (skipping): ")(str_type)
				  (".").Flush();
                            }
                        } else {
                            LogNormal(OT_METHOD)(__FUNCTION__)(
                                ": Failed in pPayment->IsValid "
                                "or pPayment->SetTempValues().")
                                .Flush();
                        }
                    }
                }  // looping through payments inbox.
                else
                    LogDetail(OT_METHOD)(__FUNCTION__)(
                        ": Failed loading payments inbox. "
                        "(Probably just doesn't exist yet).")
                        .Flush();
                // Above we compiled a list of purses, cheques / vouchers to
                // accept.
                // If there are any on that list, then ACCEPT them here.
                //
                if (!thePaymentMap.empty()) {
                    for (mapOfPayments::reverse_iterator it =
                             thePaymentMap.rbegin();
                         it != thePaymentMap.rend();
                         ++it)  // backwards since we are processing (removing)
                                // payments by index.
                    {
                        std::int32_t lIndex = it->first;
                        auto pPayment = it->second;
                        if (false == bool(pPayment)) {
                            otErr << __FUNCTION__
                                  << ": Error: payment pointer was "
                                     "nullptr! (Should never happen.) "
                                     "Skipping.\n";
                            continue;
                        }
                        auto payment = String::Factory();
                        if (!pPayment->GetPaymentContents(payment)) {
                            otErr << __FUNCTION__
                                  << ": Error: Failed while trying to "
                                     "get payment string contents. "
                                     "(Skipping.)\n";
                            continue;
                        }

                        std::map<std::int32_t, TransactionNumber>::iterator
                            it_pmnt_box_trns_num =
                                mapPaymentBoxTransNum.find(lIndex);
                        TransactionNumber lPaymentBoxTransNum = 0;

                        if (it_pmnt_box_trns_num != mapPaymentBoxTransNum.end())
                            lPaymentBoxTransNum = it_pmnt_box_trns_num->second;

                        auto paymentAssetType = Identifier::Factory();
                        bool bGotAsset = pPayment->GetInstrumentDefinitionID(
                            paymentAssetType);

                        auto paymentNotaryId = Identifier::Factory();
                        bool bGotPaymentNotary =
                            pPayment->GetNotaryID(paymentNotaryId);

                        std::string str_instrument_definition_id,
                            str_payment_notary_id;

                        if (bGotPaymentNotary) {
                            const auto strPaymentNotaryId =
                                String::Factory(paymentNotaryId);
                            str_payment_notary_id = strPaymentNotaryId->Get();
                        }
                        if (str_payment_notary_id.empty()) {
                            otErr
                                << __FUNCTION__
                                << ": Error: Failed while trying to "
                                   "get Notary ID from payment. (Skipping.)\n";
                            continue;
                        }

                        if (bGotAsset) {
                            const auto strInstrumentDefinitionID =
                                String::Factory(paymentAssetType);
                            str_instrument_definition_id =
                                strInstrumentDefinitionID->Get();
                        }
                        if (str_instrument_definition_id.empty()) {
                            otErr
                                << __FUNCTION__
                                << ": Error: Failed while trying to "
                                   "get instrument definition ID from payment. "
                                   "(Skipping.)\n";
                            continue;
                        }
                        // pick an account to deposit the cheque into.
                        for (auto& it_acct : m_accounts) {
                            const std::string& str_account_id(it_acct);
                            const auto theAccountID =
                                Identifier::Factory(str_account_id);
                            auto account = wallet_.Account(theAccountID);

                            if (false == bool(account)) {
                                // This can happen if the user erases the
                                // account. Therefore we just need to skip it.
                                LogVerbose(__FUNCTION__)(
                                    ": Skipping an account (")(str_account_id)(
                                    ") since it has disappeared from the "
                                    "wallet. (Probably deleted by the user).")
                                    .Flush();

                                continue;
                            }

                            const Identifier& theAcctNymID =
                                account.get().GetNymID();
                            const Identifier& theAcctNotaryID =
                                account.get().GetPurportedNotaryID();
                            const Identifier& theAcctInstrumentDefinitionID =
                                account.get().GetInstrumentDefinitionID();
                            const std::string str_acct_type =
                                account.get().GetTypeString();
                            account.Release();
                            const auto strAcctNymID =
                                String::Factory(theAcctNymID);
                            const auto strAcctNotaryID =
                                String::Factory(theAcctNotaryID);
                            const auto strAcctInstrumentDefinitionID =
                                String::Factory(theAcctInstrumentDefinitionID);
                            // If the current account is owned by the Nym, AND
                            // it has the same instrument definition ID
                            // as the cheque being deposited, then let's deposit
                            // the cheque into that account.
                            //
                            // TODO: we should first just see if the default
                            // account matches, instead of doing
                            // this loop in the first place.
                            //
                            if ((theNymID == theAcctNymID) &&
                                (strAcctNotaryID->Compare(
                                    str_payment_notary_id.c_str())) &&
                                (strAcctInstrumentDefinitionID->Compare(
                                    str_instrument_definition_id.c_str())) &&
                                // No issuer accounts allowed here. User only.
                                (0 == str_acct_type.compare("user"))) {
                                // Accept it.
                                //
                                auto strIndices = String::Factory();
                                strIndices->Format("%d", lIndex);
                                const std::string str_indices(
                                    strIndices->Get());

                                std::string str_server_response;

                                if (!accept_from_paymentbox(
                                        str_msg_notary_id,
                                        str_account_id,
                                        str_indices,
                                        "ANY",
                                        &str_server_response)) {
                                    otErr << __FUNCTION__
                                          << ": Error while trying to "
                                             "accept this instrument.\n";
                                } else {
                                    TransactionNumber temp_number = 0;
                                    TransactionNumber temp_trans_number = 0;
                                    TransactionNumber temp_display_number = 0;

                                    if (pPayment->GetTransactionNum(
                                            temp_number))
                                        temp_trans_number = temp_number;
                                    if (pPayment->GetTransNumDisplay(
                                            temp_number))
                                        temp_display_number = temp_number;

                                    TransactionNumber display_number =
                                        (temp_display_number > 0)
                                            ? temp_display_number
                                            : temp_trans_number;

                                    // Last resort here. The number in my
                                    // payment box is one that is guaranteed I
                                    // will never be able to match up with a
                                    // number in anyone else's payment box.
                                    // So what use is it for display? It's also
                                    // guaranteed that multiple of my receipts
                                    // will NEVER be able to match up with each
                                    // other in the payments table in the
                                    // Moneychanger DB, since they will all have
                                    // different display numbers.
                                    //
                                    if (display_number <= 0)
                                        display_number = lPaymentBoxTransNum;
                                }
                                break;
                            }
                        }  // loop through accounts to find one to deposit
                           // cheque
                           // into.
                    }  // Loop through payments to deposit.
                    // Empty the list and delete the payments inside.
                    thePaymentMap.clear();
                }  // if (!thePaymentMap.empty())
            }
        }
    }

    // ASSET ACCOUNT -- INBOX
    //
    // Loop through the Accounts.
    //
    if (m_bAutoAcceptReceipts || m_bAutoAcceptTransfers) {
        std::int32_t nAccountIndex = -1;
        for (auto& it_acct : m_accounts) {
            ++nAccountIndex;  // (0 on first iteration.)

            if (0 == nAccountIndex) {
                LogVerbose("---------------------------------").Flush();
                LogVerbose(__FUNCTION__)(
                    ": Beginning auto-accept loop through the "
                    "accounts in the wallet...")
                    .Flush();
            }

            // For each account, loop through its inbox, outbox, and record box.
            const std::string& str_account_id(it_acct);
            const auto theAccountID = Identifier::Factory(str_account_id);
            auto account = wallet_.Account(theAccountID);

            if (false == bool(account)) {
                // This can happen if the user erases the account.
                // Therefore we just need to skip it.
                LogVerbose(__FUNCTION__)(": Skipping an account (")(
                    str_account_id)(") since it has disappeared from the "
                                    "wallet. (Probably deleted by "
                                    "the user).")
                    .Flush();

                continue;
            }
            const Identifier& theNymID = account.get().GetNymID();
            const Identifier& theNotaryID =
                account.get().GetPurportedNotaryID();
            const Identifier& theInstrumentDefinitionID =
                account.get().GetInstrumentDefinitionID();
            account.Release();
            const auto strNymID = String::Factory(theNymID);
            const auto strNotaryID = String::Factory(theNotaryID);
            const auto strInstrumentDefinitionID =
                String::Factory(theInstrumentDefinitionID);
            LogVerbose("------------").Flush();
            LogVerbose(__FUNCTION__)(": Account: ")(nAccountIndex)(", ID: ")(
	       str_account_id)(".")
                .Flush();
            const std::string str_nym_id(strNymID->Get());
            const std::string str_notary_id(strNotaryID->Get());
            const std::string str_instrument_definition_id(
                strInstrumentDefinitionID->Get());
            // NOTE: Since this account is already on my "care about" list for
            // accounts,
            // I wouldn't bother double-checking my "care about" lists for
            // servers, nyms,
            // and instrument definitions. But I still look up the appropriate
            // string for
            // each, since
            // I have to pass a reference to it into the constructor for
            // Record. (To a version
            // that won't be deleted, since the Record will reference it. And
            // the "care about"
            // list definitely contains a copy of the string that won't be
            // deleted.)
            //
            auto it_nym = std::find(m_nyms.begin(), m_nyms.end(), str_nym_id);
            auto it_server =
                std::find(m_servers.begin(), m_servers.end(), str_notary_id);
            auto it_asset = m_assets.find(str_instrument_definition_id);
            if ((m_nyms.end() == it_nym) || (m_servers.end() == it_server) ||
                (m_assets.end() == it_asset)) {
                LogVerbose(__FUNCTION__)(": Skipping an account (")(
                    str_account_id)(") since its Nym, or Server, or Asset Type "
                                    "wasn't on my list.")
                    .Flush();

                continue;
            }
            // Loop through asset account INBOX.
            //
            // OPTIMIZE FYI:
            // NOTE: LoadInbox is much SLOWER than LoadInboxNoVerify, but it
            // also lets you get
            // the NAME off of the box receipt. So if you are willing to GIVE UP
            // the NAME, in
            // return for FASTER PERFORMANCE, then call SetFastMode() before
            // Populating.

            std::shared_ptr<Ledger> pInbox{nullptr};

            if (false == theNymID.empty()) {
                pInbox = m_bRunFast
                             ? Opentxs::Client().OTAPI().LoadInboxNoVerify(
                                   theNotaryID, theNymID, theAccountID)
                             : Opentxs::Client().OTAPI().LoadInbox(
                                   theNotaryID, theNymID, theAccountID);
            }

            if (false == bool(pInbox)) {
                LogVerbose(__FUNCTION__)(": Skipping an account (")(
                    str_account_id)(") since its inbox failed to load (have "
                                    "you downloaded the latest "
                                    "one?)")
                    .Flush();

                continue;
            }
            const auto strInbox = String::Factory(*pInbox);
            const std::string str_inbox(strInbox->Get());
            bool bFoundAnyToAccept = false;
            std::string strResponseLedger;
            std::int32_t nInboxIndex = -1;
            // It loaded up, so let's loop through it.
            for (auto& it : pInbox->GetTransactionMap()) {
                ++nInboxIndex;  // (0 on first iteration.)

                if (0 == nInboxIndex) {
                    LogVerbose(__FUNCTION__)(
                        ": Beginning loop through asset account INBOX...")
                        .Flush();
                }

                auto pBoxTrans = it.second;

                OT_ASSERT(false != bool(pBoxTrans));

                LogVerbose(__FUNCTION__)(": Inbox index: ")(nInboxIndex)
                    .Flush();
                const std::string str_type(
                    pBoxTrans->GetTypeString());  // pending, chequeReceipt,
                                                  // etc.
                const bool bIsTransfer =
                    (transactionType::pending == pBoxTrans->GetType());
                const bool bIsReceipt = !bIsTransfer;
                if ((m_bAutoAcceptReceipts && bIsReceipt) ||
                    (m_bAutoAcceptTransfers && bIsTransfer)) {
                    LogVerbose(__FUNCTION__)(": Auto-accepting: incoming ")(
                        bIsTransfer ? "pending transfer"
                                    : "receipt")(" (str_type: ")(str_type)(")")
                        .Flush();
                    // If we haven't found any yet, then this must be the first
                    // one!
                    //
                    if (!bFoundAnyToAccept) {
                        bFoundAnyToAccept = true;

                        std::int32_t nNumberNeeded =
                            20;  // I'm just hardcoding: "Make sure I have at
                                 // least 20 transaction numbers."
                        {
                            if (!Opentxs::Client()
                                     .ServerAction()
                                     .GetTransactionNumbers(
                                         theNymID,
                                         theNotaryID,
                                         nNumberNeeded)) {
                                LogNormal(OT_METHOD)(__FUNCTION__)(
                                    ": Failure: "
                                    "make_sure_enough_trans_nums: "
                                    "returned false. (Skipping inbox "
                                    "for account ")(str_account_id.c_str())
				    (").").Flush();
                                continue;
                            }
                        }
                        strResponseLedger =
                            Opentxs::Client().Exec().Ledger_CreateResponse(
                                str_notary_id, str_nym_id, str_account_id);

                        if (strResponseLedger.empty()) {
                            LogNormal(OT_METHOD)(__FUNCTION__)(
                                ": Failure: "
                                "OT_API_Ledger_CreateResponse "
                                "returned nullptr. (Skipping inbox "
                                "for account ")(str_account_id.c_str())(").")
                                .Flush();
                            continue;
                        }
                    }
                    const auto strTrans = String::Factory(*pBoxTrans);
                    const std::string str_trans(strTrans->Get());
                    std::string strNEW_ResponseLEDGER =
                        Opentxs::Client().Exec().Transaction_CreateResponse(
                            str_notary_id,
                            str_nym_id,
                            str_account_id,
                            strResponseLedger,
                            str_trans,
                            true);  // accept = true (versus rejecting a pending
                                    // transfer, for example.)

                    if (strNEW_ResponseLEDGER.empty()) {
                        LogNormal(OT_METHOD)(__FUNCTION__)(
                            ": Failure: "
                            "OT_API_Transaction_CreateResponse "
                            "returned nullptr. (Skipping inbox for "
                            "account ")(str_account_id.c_str())(").")
                            .Flush();
                        continue;
                    }
                    strResponseLedger = strNEW_ResponseLEDGER;
                }
            }  // For
            // Okay now we have the response ledger all ready to go, let's
            // process it!
            //
            if (bFoundAnyToAccept && !strResponseLedger.empty()) {
                std::string strFinalizedResponse =
                    Opentxs::Client().Exec().Ledger_FinalizeResponse(
                        str_notary_id,
                        str_nym_id,
                        str_account_id,
                        strResponseLedger);

                if (strFinalizedResponse.empty()) {
                    LogNormal(OT_METHOD)(__FUNCTION__)(
                        ": Failure: "
                        "OT_API_Ledger_FinalizeResponse returned "
                        "nullptr. (Skipping inbox for account ")(
                        str_account_id.c_str())(").")
                        .Flush();
                    continue;
                }
                // Server communications are handled here...
                //
                auto ledger{Opentxs::Client().Factory().Ledger(
                    theNymID, theAccountID, theNotaryID)};

                OT_ASSERT(false != bool(ledger));

                const auto loaded = ledger->LoadLedgerFromString(
                    String::Factory(strFinalizedResponse));

                OT_ASSERT(loaded);

                std::string strResponse;
                {
                    strResponse =
                        Opentxs::Client()
                            .ServerAction()
                            .ProcessInbox(
                                theNymID, theNotaryID, theAccountID, ledger)
                            ->Run();
                }
                std::string strAttempt = "process_inbox";

                std::int32_t nInterpretReply = InterpretTransactionMsgReply(
                    Opentxs::Client(),
                    str_notary_id,
                    str_nym_id,
                    str_account_id,
                    strAttempt,
                    strResponse);

                if (1 == nInterpretReply) {
                    // Download all the intermediary files (account balance,
                    // inbox, outbox, etc)
                    // since they have probably changed from this operation.
                    //
                    bool bRetrieved =
                        Opentxs::Client().ServerAction().DownloadAccount(
                            theNymID,
                            theNotaryID,
                            theAccountID,
                            true);  // bForceDownload defaults to false.

                    LogVerbose("Server response (")(strAttempt)(
                        "): SUCCESS processing/accepting inbox.")
                        .Flush();
                    LogVerbose(bRetrieved ? "Success" : "Failed")(
                        " retrieving intermediary files for account.")
                        .Flush();
                }
            }
        }
    }
    return true;
}

// POPULATE:

// Populates m_contents from OT API. Calls ClearContents().

bool RecordList::Populate()
{
    ClearContents();
    // Loop through all the accounts.
    //
    // From Open-Transactions.h:
    // Opentxs::Client().OTAPI().GetServerCount()
    //
    // From OTAPI.h:
    // SwigWrap::GetServerCount()  // wraps the above call.
    //
    // Before populating, process out any items we're supposed to accept
    // automatically.
    //
    PerformAutoAccept();
    // OUTPAYMENTS, OUTMAIL, MAIL, PAYMENTS INBOX, and RECORD BOX (2 kinds.)
    // Loop through the Nyms.
    //
    std::int32_t nNymIndex = -1;
    for (auto& it_nym : m_nyms) {
        ++nNymIndex;

        if (0 == nNymIndex) {
            LogVerbose("=============== ")(__FUNCTION__)(
                ": Beginning loop through Nyms...")
                .Flush();
        }

        const std::string& str_nym_id(it_nym);
        const auto theNymID = Identifier::Factory(str_nym_id);
        ConstNym pNym = wallet_.Nym(theNymID);
        if (!pNym) continue;

        // For each Nym, loop through his OUTPAYMENTS box.
        //
        const std::int32_t nOutpaymentsCount =
            SwigWrap::GetNym_OutpaymentsCount(str_nym_id);
        LogVerbose("--------").Flush();
        LogVerbose(__FUNCTION__)(": Nym ")(nNymIndex)(", nOutpaymentsCount: ")(
            nOutpaymentsCount)(", ID: ")(theNymID)
            .Flush();

        for (std::int32_t nCurrentOutpayment = 0;
             nCurrentOutpayment < nOutpaymentsCount;
             ++nCurrentOutpayment) {
            LogVerbose(OT_METHOD)(__FUNCTION__)(": Outpayment instrument: ")(
                nCurrentOutpayment)(".")
                .Flush();
            const auto strOutpayment =
                String::Factory(SwigWrap::GetNym_OutpaymentsContentsByIndex(
                    str_nym_id, nCurrentOutpayment));
            std::string str_memo;
            auto theOutPayment{
                Opentxs::Client().Factory().Payment(strOutpayment)};

            OT_ASSERT(false != bool(theOutPayment));

            if (!theOutPayment->IsValid() || !theOutPayment->SetTempValues()) {
                otErr << __FUNCTION__
                      << ": Skipping: Unable to load outpayments "
                         "instrument from string:\n"
                      << strOutpayment->Get() << "\n";
                continue;
            }
            Amount lAmount = 0;
            std::string str_amount;  // <========== AMOUNT

            if (theOutPayment->GetAmount(lAmount)) {
                if (((OTPayment::CHEQUE == theOutPayment->GetType()) ||
                     (OTPayment::PURSE == theOutPayment->GetType()) ||
                     (OTPayment::VOUCHER == theOutPayment->GetType())) &&
                    (lAmount > 0))
                    lAmount *= (-1);

                if ((OTPayment::INVOICE == theOutPayment->GetType()) &&
                    (lAmount < 0))
                    lAmount *= (-1);

                auto strTemp = String::Factory();
                strTemp->Format("%" PRId64 "", lAmount);
                str_amount = strTemp->Get();
            }
            auto theInstrumentDefinitionID = Identifier::Factory();
            const std::string* p_str_asset_type =
                &RecordList::s_blank;  // <========== ASSET TYPE
            const std::string* p_str_asset_name =
                &RecordList::s_blank;  // instrument definition display name.
            std::string str_outpmt_asset;  // The instrument definition we found
                                           // on the payment (if we found
                                           // anything.)
            // --------------------------------------------------
            auto paymentNotaryId = Identifier::Factory();
            const bool bGotPaymentNotary =
                theOutPayment->GetNotaryID(paymentNotaryId);

            std::string str_outpayment_notary_id;

            if (bGotPaymentNotary) {
                str_outpayment_notary_id = paymentNotaryId->str();
            }
            if (str_outpayment_notary_id.empty()) {
                otErr << __FUNCTION__
                      << ": Error: Failed while trying to "
                         "get Notary ID from payment. (Skipping.)\n";
                continue;
            }
            // --------------------------------------------------
            if (theOutPayment->GetInstrumentDefinitionID(
                    theInstrumentDefinitionID)) {
                auto strTemp = String::Factory(theInstrumentDefinitionID);
                str_outpmt_asset = strTemp->Get();
                auto it_asset = m_assets.find(str_outpmt_asset);
                if (it_asset != m_assets.end())  // Found it on the map of asset
                                                 // types we care about.
                {
                    p_str_asset_type = &(
                        it_asset->first);  // Set the instrument definition ID.
                    p_str_asset_name = &(it_asset->second);  // The CurrencyTLA.
                                                             // Examples: USD,
                                                             // BTC, etc.
                } else {
                    // There was definitely an instrument definition on the
                    // instrument, and it definitely did not match any of the
                    // assets that we care about. Therefore, skip.
                    //
                    LogVerbose(OT_METHOD)(__FUNCTION__)(
                        ": Skipping outpayment (we don't care "
                        "about this unit type ")(str_outpmt_asset.c_str())(").")
                        .Flush();

                    otErr << __FUNCTION__
                          << ": Skipping outpayment (we don't care "
                             "about this unit type "
                          << str_outpmt_asset.c_str() << ")\n";

                    continue;
                }
            }
            // By this point, p_str_asset_type and p_str_asset_name are
            // definitely set.
            OT_ASSERT(nullptr != p_str_asset_type);  // and it's either blank,
                                                     // or
            // it's one of the instrument definitions
            // we care about.
            OT_ASSERT(nullptr != p_str_asset_name);  // and it's either blank,
                                                     // or
            // it's one of the instrument definitions
            // we care about.
            auto theAccountID = Identifier::Factory();
            const std::string* p_str_account =
                &RecordList::s_blank;        // <========== ACCOUNT
            std::string str_outpmt_account;  // The accountID we found on the
                                             // payment (if we found anything.)

            // (In Outpayments, the SENDER's account is MY acct.)
            // UPDATE: UNLESS IT'S A PAYMENT PLAN. In which case the "Sender" of
            // money is the payer aka customer aka the RECIPIENT of the payment
            // plan proposal that was sent to him from the merchant. In that
            // case, the merchant is the "SENDER" of the proposal but on the
            // financial instrument the customer is the "SENDER" of the money!
            // RecordList needs to handle this case so that the user can see
            // his outbox contents displayed properly.
            //
            if ((theOutPayment->IsPaymentPlan() &&
                 theOutPayment->GetRecipientAcctID(theAccountID)) ||
                // Otherwise if it's not a payment plan then we're looking
                // for the Sender (not Recipient) account. See above comment.
                // Also: Since Nym is ME, the Account must be MY account.
                //
                theOutPayment->GetSenderAcctIDForDisplay(theAccountID)) {
                str_outpmt_account = theAccountID->str();
                auto it_acct = std::find(
                    m_accounts.begin(), m_accounts.end(), str_outpmt_account);
                if (it_acct != m_accounts.end())  // Found it on the list of
                                                  // accounts we care about.
                {
                    p_str_account = &(*it_acct);
                }
                // We don't skip vouchers since the sender account (e.g. the
                // server's account) is definitely not one of my accounts --
                // so the voucher would end up getting skipped every single
                // time.
                //              else if (OTPayment::VOUCHER !=
                //              theOutPayment->GetType())
                else {
                    // There was definitely an account on the instrument, and it
                    // definitely did not match any of the accounts that we care
                    // about. Therefore, skip.
                    //
                    LogVerbose(OT_METHOD)(__FUNCTION__)(
                        ": Skipping outpayment (we don't care "
                        "about this account ")(str_outpmt_account.c_str())(").")
                        .Flush();

                    otErr << __FUNCTION__
                          << ": Skipping outpayment (we don't care "
                             "about this account "
                          << str_outpmt_account.c_str() << ")\n";
                    continue;
                }
            }
            // By this point, p_str_account is definitely set.
            OT_ASSERT(nullptr != p_str_account);  // and it's either blank, or
                                                  // it's one of the accounts we
                                                  // care about.
            // strOutpayment contains the actual outgoing payment instrument.
            //
            const std::string str_outpmt_transport_server =
                SwigWrap::GetNym_OutpaymentsNotaryIDByIndex(
                    str_nym_id, nCurrentOutpayment);
            const std::string str_outpmt_recipientID =  // Notice here, unlike
                                                        // the sender account id
                                                        // above (which is
                                                        // gleaned from the
                                                        // instrument itself)
                SwigWrap::GetNym_OutpaymentsRecipientIDByIndex(  // instead we
                                                                 // get the
                                                                 // recipient
                                                                 // Nym ID
                                                                 // from the
                                                                 // outgoing
                                                                 // message.
                                                                 // This is
                                                                 // good,
                                                                 // because
                    str_nym_id,
                    nCurrentOutpayment);  // otherwise we'd have to check to see
                                          // if it's a payment plan here, and
                                          // get the "sender" (payer aka
                                          // customer) NymID and set it here as
                                          // the recipient! (Since the merchant
                                          // "sends" the proposal in a message
                                          // to the customer, who then "sends"
                                          // the payments to the merchant acct.)
                                          // See above comment about this.
            // str_outpmt_server is the server for this outpayment.
            // But is that server on our list of servers that we care about?
            // Let's see if that server is on m_servers (otherwise we can skip
            // it.)
            // Also, let's do the same for instrument definitions.
            //
            auto it_server = std::find(
                m_servers.begin(),
                m_servers.end(),
                str_outpmt_transport_server);

            if (it_server != m_servers.end())  // Found the notaryID on the list
                                               // of servers we care about.
            {
                // TODO OPTIMIZE: instead of looking up the Nym's name every
                // time, look it up ONCE when first adding the NymID. Add it to
                //  a map, instead of a list, and add the Nym's name as the
                // second item in the map's pair. (Just like I already did with
                // the instrument definition.)
                //
                auto strNameTemp = String::Factory();
                std::string str_name;
                strNameTemp->Format(
                    RecordList::textTo(), str_outpmt_recipientID.c_str());

                str_name = strNameTemp->Get();
                auto strMemo = String::Factory();
                if (theOutPayment->GetMemo(strMemo)) {
                    str_memo = strMemo->Get();
                }
                // For the "date" on this record we're using the "valid from"
                // date on the instrument.
                std::string str_date = "0";
                time64_t tFrom = OT_TIME_ZERO;
                time64_t tTo = OT_TIME_ZERO;

                if (theOutPayment->GetValidFrom(tFrom)) {
                    const std::uint64_t lFrom = OTTimeGetSecondsFromTime(tFrom);
                    auto strFrom = String::Factory();
                    strFrom->Format("%" PRIu64 "", lFrom);
                    str_date = strFrom->Get();
                }
                theOutPayment->GetValidTo(tTo);
                // Instrument type (cheque, voucher, etc)
                //
                std::int32_t nType =
                    static_cast<int32_t>(theOutPayment->GetType());

                const std::string& str_type = GetTypeString(nType);
                // CREATE A Record AND POPULATE IT...
                //
                LogVerbose(OT_METHOD)(__FUNCTION__)(
                    ": ADDED: pending outgoing instrument (str_type: ")(
                    str_type.c_str())(").")
                    .Flush();

                shared_ptr_Record sp_Record(new Record(
                    *this,
                    str_outpmt_transport_server,  // Transport Notary
                    str_outpayment_notary_id,     // The Payment Notary
                    *p_str_asset_type,
                    *p_str_asset_name,
                    str_nym_id,      // This is the Nym WHOSE BOX IT IS.
                    *p_str_account,  // This is the Nym's account according
                                     // to
                    // the payment instrument, IF that account
                    // was found on our list of accounts we care
                    // about. Or it's blank if no account was
                    // found on the payment instrument.
                    // Everything above this line, it stores a reference to
                    // an
                    // external string.
                    // Everything below this line, it makes its own internal
                    // copy of the string.
                    str_name,  // name of recipient (since its in
                               // outpayments
                               // box.)
                    str_date,  // the "valid from" date on the instrument.
                    str_amount,
                    str_type,  // cheque, voucher, smart contract, etc
                    true,      // bIsPending=true since its in the outpayments
                               // box.
                    true,      // bIsOutgoing=true. Otherwise it'd be in record
                               // box
                               // already.
                    false,     // IsRecord
                    false,     // IsReceipt
                    Record::Instrument));
                sp_Record->SetContents(strOutpayment->Get());
                sp_Record->SetOtherNymID(str_outpmt_recipientID);
                if (!str_memo.empty()) sp_Record->SetMemo(str_memo);
                sp_Record->SetDateRange(tFrom, tTo);
                sp_Record->SetBoxIndex(nCurrentOutpayment);

                TransactionNumber lTransNum = 0, lTransNumDisplay = 0;
                theOutPayment->GetOpeningNum(lTransNum, theNymID);
                theOutPayment->GetTransNumDisplay(lTransNumDisplay);

                if (lTransNumDisplay <= 0) lTransNumDisplay = lTransNum;

                sp_Record->SetTransactionNum(lTransNum);
                sp_Record->SetTransNumForDisplay(lTransNumDisplay);

                m_contents.push_back(sp_Record);

                //                otErr << "DEBUGGING! Added pending outgoing: "
                //                << str_type.c_str() << "."
                //                "\n lTransNum: " << lTransNum << "\n";

            } else  // the server for this outpayment is not on the list of
                    // servers we care about. Skip this outpayment.
            {
                LogVerbose(OT_METHOD)(__FUNCTION__)(
                    ": Skipping outgoing instrument (we don't "
                    "care about this notary ")(
                    str_outpayment_notary_id.c_str())(").")
                    .Flush();
                continue;
            }
        }  // for outpayments.
        // --------------------------------------------------
        if (!m_bIgnoreMail) {
            // For each Nym, loop through his MAIL box.
            auto& exec = Opentxs::Client().Exec();
            const auto mail = exec.GetNym_MailCount(str_nym_id);
            std::int32_t index = 0;

            for (const auto& id : mail) {
                LogVerbose(OT_METHOD)(__FUNCTION__)(": Mail index: ")(index)(
                    ".")
                    .Flush();
                const auto nymID = Identifier::Factory(str_nym_id);

                if (id.empty()) {
                    index++;

                    continue;
                }

                auto message = Opentxs::Client().Activity().Mail(
                    nymID, Identifier::Factory(id), StorageBox::MAILINBOX);

                if (!message) {
                    otErr << __FUNCTION__
                          << ": Failed to load mail message with "
                          << "ID " << id << " from inbox." << std::endl;
                    index++;

                    continue;
                }

                OT_ASSERT(message);

                const std::string str_mail_server =
                    exec.GetNym_MailNotaryIDByIndex(str_nym_id, id);
                const std::string str_mail_senderID =
                    exec.GetNym_MailSenderIDByIndex(str_nym_id, id);
                // str_mail_server is the server for this mail.
                // But is that server on our list of servers that we care about?
                // Let's see if that server is on m_servers (otherwise we can
                // skip it.)
                //
                auto it_server = std::find(
                    m_servers.begin(), m_servers.end(), str_mail_server);

                if (it_server != m_servers.end())  // Found the notaryID on the
                                                   // list of servers we care
                                                   // about.
                {
                    // TODO OPTIMIZE: instead of looking up the Nym's name every
                    // time, look it
                    // up ONCE when first adding the NymID. Add it to a map,
                    // instead of a list, and add the Nym's name as the second
                    // item in the map's pair. (Just like I already did with the
                    // instrument definition.)
                    //
                    auto strNameTemp = String::Factory();
                    std::string str_name;
                    strNameTemp->Format(
                        RecordList::textFrom(), str_mail_senderID.c_str());

                    str_name = strNameTemp->Get();
                    const std::string* p_str_asset_type =
                        &RecordList::s_blank;  // <========== ASSET TYPE
                    const std::string* p_str_asset_name =
                        &RecordList::s_blank;  // instrument definition
                                               // display name.
                    const std::string* p_str_account =
                        &RecordList::s_blank;  // <========== ACCOUNT

                    std::string str_amount;  // There IS NO amount, on mail. (So
                                             // we leave this empty.)

                    std::uint64_t lDate = message->m_lTime;
                    auto strDate = String::Factory();
                    strDate->Format("%" PRIu64 "", lDate);
                    const std::string str_date(strDate->Get());
                    // CREATE A Record AND POPULATE IT...
                    //
                    LogVerbose(OT_METHOD)(__FUNCTION__)(
                        ": ADDED: incoming mail.")
                        .Flush();

                    shared_ptr_Record sp_Record(new Record(
                        *this,
                        *it_server,           // Transport Notary
                        RecordList::s_blank,  // Payment notary blank (mail)
                        *p_str_asset_type,
                        *p_str_asset_name,
                        str_nym_id,      // This is the Nym WHOSE BOX IT IS.
                        *p_str_account,  // This is the Nym's account according
                                         // to
                        // the payment instrument, IF that account
                        // was found on our list of accounts we care
                        // about. Or it's blank if no account was
                        // found on the payment instrument.
                        // Everything above this line, it stores a reference to
                        // an
                        // external string.
                        // Everything below this line, it makes its own
                        // internal copy of the string.
                        str_name,  // name of sender (since its in incoming mail
                                   // box.)
                        str_date,  // How do we get the date from a mail?
                        str_amount,
                        RecordList::s_message_type,  // "message"
                        false,  // bIsPending=false since its already received.
                        false,  // bIsOutgoing=false. It's incoming mail, not
                                // outgoing mail.
                        false,  // IsRecord
                        false,  // IsReceipt
                        Record::Mail));
                    const auto strMail = String::Factory(
                        SwigWrap::GetNym_MailContentsByIndex(str_nym_id, id));
                    sp_Record->SetContents(strMail->Get());
                    sp_Record->SetOtherNymID(str_mail_senderID);
                    sp_Record->SetBoxIndex(index);
                    sp_Record->SetThreadItemId(id);
                    sp_Record->SetDateRange(
                        OTTimeGetTimeFromSeconds(message->m_lTime),
                        OTTimeGetTimeFromSeconds(message->m_lTime));
                    m_contents.push_back(sp_Record);
                }

                index++;
            }  // loop through incoming Mail.
            // Outmail
            //
            const auto outmail = exec.GetNym_OutmailCount(str_nym_id);
            index = 0;

            for (const auto& id : outmail) {
                LogVerbose(OT_METHOD)(__FUNCTION__)(": Outmail index: ")(index)(
                    ".")
                    .Flush();
                const auto nymID = Identifier::Factory(str_nym_id);

                if (id.empty()) {
                    index++;

                    continue;
                }

                auto message = Opentxs::Client().Activity().Mail(
                    nymID, Identifier::Factory(id), StorageBox::MAILOUTBOX);

                if (!message) {
                    otErr << __FUNCTION__
                          << ": Failed to load mail message with "
                          << "ID " << id << " from outbox." << std::endl;
                    index++;

                    continue;
                }

                OT_ASSERT(message);

                const std::string str_mail_server =
                    SwigWrap::GetNym_OutmailNotaryIDByIndex(str_nym_id, id);
                const std::string str_mail_recipientID =
                    SwigWrap::GetNym_OutmailRecipientIDByIndex(str_nym_id, id);
                // str_mail_server is the server for this mail.
                // But is that server on our list of servers that we care about?
                // Let's see if that server is on m_servers (otherwise we can
                // skip it.)
                //
                auto it_server = std::find(
                    m_servers.begin(), m_servers.end(), str_mail_server);

                if (it_server != m_servers.end())  // Found the notaryID on the
                                                   // list of servers we care
                                                   // about.
                {
                    // TODO OPTIMIZE: instead of looking up the Nym's name every
                    // time, look it
                    // up ONCE when first adding the NymID. Add it to a map,
                    // instead of a list, and add the Nym's name as the second
                    // item in the map's pair. (Just like I already did with the
                    // instrument definition.)
                    //
                    auto strNameTemp = String::Factory();
                    std::string str_name;
                    strNameTemp->Format(
                        RecordList::textTo(), str_mail_recipientID.c_str());

                    str_name = strNameTemp->Get();
                    const std::string* p_str_asset_type =
                        &RecordList::s_blank;  // <========== ASSET TYPE
                    const std::string* p_str_asset_name =
                        &RecordList::s_blank;  // instrument definition
                                               // display name.
                    const std::string* p_str_account =
                        &RecordList::s_blank;  // <========== ACCOUNT

                    std::string str_amount;  // There IS NO amount, on mail. (So
                                             // we leave this empty.)

                    std::uint64_t lDate = message->m_lTime;
                    auto strDate = String::Factory();
                    strDate->Format("%" PRIu64 "", lDate);
                    const std::string str_date(strDate->Get());
                    // CREATE A Record AND POPULATE IT...
                    //
                    LogVerbose(OT_METHOD)(__FUNCTION__)(": ADDED: sent mail.")
                        .Flush();

                    shared_ptr_Record sp_Record(new Record(
                        *this,
                        *it_server,           // Transport Notary
                        RecordList::s_blank,  // Payment notary blank (mail)
                        *p_str_asset_type,
                        *p_str_asset_name,
                        str_nym_id,      // This is the Nym WHOSE BOX IT IS.
                        *p_str_account,  // This is the Nym's account according
                                         // to
                        // the payment instrument, IF that account
                        // was found on our list of accounts we care
                        // about. Or it's blank if no account was
                        // found on the payment instrument.
                        // Everything above this line, it stores a reference to
                        // an
                        // external string.
                        // Everything below this line, it makes its own
                        // internal copy of the string.
                        str_name,  // name of recipient (since its in outgoing
                                   // mail
                                   // box.)
                        str_date,  // How do we get the date from a mail?
                        str_amount,
                        RecordList::s_message_type,  // "message"
                        false,  // bIsPending=false since its already sent.
                        true,   // bIsOutgoing=true. It's OUTGOING mail.
                        false,  // IsRecord (it's not in the record box.)
                        false,  // IsReceipt
                        Record::Mail));
                    const auto strOutmail =
                        String::Factory(SwigWrap::GetNym_OutmailContentsByIndex(
                            str_nym_id, id));
                    sp_Record->SetContents(strOutmail->Get());
                    sp_Record->SetThreadItemId(id);
                    sp_Record->SetBoxIndex(index);
                    sp_Record->SetOtherNymID(str_mail_recipientID);
                    sp_Record->SetDateRange(
                        OTTimeGetTimeFromSeconds(message->m_lTime),
                        OTTimeGetTimeFromSeconds(message->m_lTime));
                    m_contents.push_back(sp_Record);
                }
                index++;
            }  // loop through outgoing Mail.
        }      // If not ignoring mail.

        // For each nym, for each server, loop through its payments inbox and
        // record box.
        //
        std::int32_t nServerIndex = -1;
        for (auto& it_server : m_servers) {
            ++nServerIndex;
            const auto theMsgNotaryID = Identifier::Factory(it_server);
            auto pServer = wallet_.Server(theMsgNotaryID);
            if (!pServer) {
                // This can happen if the user erases the server contract
                // from the wallet. Therefore we just need to skip it.
                LogVerbose(OT_METHOD)(__FUNCTION__)(
                    ": Skipping a notary server (")(it_server.c_str())(
                    ") since the contract has disappeared from the "
                    "wallet. (Probably deleted by the user.).")
                    .Flush();
                continue;
            }
            const auto strMsgNotaryID = String::Factory(theMsgNotaryID);
            LogVerbose(OT_METHOD)(__FUNCTION__)(": Transport Notary ")(
                nServerIndex)(", ID: ")(strMsgNotaryID->Get())(".")
                .Flush();
            // OPTIMIZE FYI:
            // The "NoVerify" version is much faster, but you will lose the
            // ability to get the
            // sender/recipient name from the receipts in the box. The code
            // will, however, work
            // either way.
            std::unique_ptr<Ledger> pInbox{nullptr};

            if (false == theNymID->empty()) {
                pInbox =
                    m_bRunFast
                        ? Opentxs::Client().OTAPI().LoadPaymentInboxNoVerify(
                              theMsgNotaryID, theNymID)
                        : Opentxs::Client().OTAPI().LoadPaymentInbox(
                              theMsgNotaryID, theNymID);
            }

            std::int32_t nIndex = (-1);
            // It loaded up, so let's loop through it.
            if (false != bool(pInbox)) {
                for (auto& it : pInbox->GetTransactionMap()) {
                    const TransactionNumber lReceiptId = it.first;
                    auto pBoxTrans = it.second;
                    OT_ASSERT(false != bool(pBoxTrans));
                    ++nIndex;  // 0 on first iteration.
                    LogVerbose(OT_METHOD)(__FUNCTION__)(": Incoming payment: ")(
                        nIndex)(".")
                        .Flush();
                    std::string str_name;  // name of sender (since its in the
                                           // payments inbox.)
                    std::string str_sender_nym_id;
                    std::string str_sender_acct_id;

                    std::string str_payment_notary;
                    const std::string str_transport_notary =
                        strMsgNotaryID->Get();

                    if (!pBoxTrans->IsAbbreviated()) {
                        auto theSenderID = Identifier::Factory();

                        if (pBoxTrans->GetSenderNymIDForDisplay(theSenderID)) {
                            const auto strSenderID =
                                String::Factory(theSenderID);
                            str_sender_nym_id = strSenderID->Get();
                            auto strNameTemp = String::Factory();
                            strNameTemp->Format(
                                RecordList::textFrom(),
                                str_sender_nym_id.c_str());
                            str_name = strNameTemp->Get();
                        }

                        theSenderID->Release();

                        if (pBoxTrans->GetSenderAcctIDForDisplay(theSenderID)) {
                            const auto strSenderID =
                                String::Factory(theSenderID);
                            str_sender_acct_id = strSenderID->Get();
                        }
                    }
                    time64_t tValidFrom = OT_TIME_ZERO, tValidTo = OT_TIME_ZERO;
                    std::string str_date =
                        "0";  // the "date signed" on the transaction receipt.
                    time64_t tDateSigned = pBoxTrans->GetDateSigned();

                    if (tDateSigned > OT_TIME_ZERO) {
                        tValidFrom = tDateSigned;
                        const std::uint64_t lDateSigned =
                            OTTimeGetSecondsFromTime(tDateSigned);
                        auto strDateSigned = String::Factory();
                        strDateSigned->Format("%" PRIu64 "", lDateSigned);
                        str_date = strDateSigned->Get();
                    }
                    const std::string* p_str_asset_type =
                        &RecordList::s_blank;  // <========== ASSET TYPE
                    const std::string* p_str_asset_name =
                        &RecordList::s_blank;  // instrument definition
                                               // display name.
                    std::string str_amount;    // <========== AMOUNT
                    std::string str_type;      // Instrument type.
                    std::string str_memo;
                    auto strContents =
                        String::Factory();  // Instrument contents.

                    TransactionNumber lPaymentInstrumentTransNumDisplay = 0;

                    if (pBoxTrans->IsAbbreviated()) {
                        str_type =
                            pBoxTrans->GetTypeString();  // instrumentNotice,
                                                         // etc.
                        Amount lAmount = pBoxTrans->GetAbbrevDisplayAmount();

                        if (0 != lAmount) {
                            auto strTemp = String::Factory();
                            strTemp->Format("%" PRId64 "", lAmount);
                            str_amount = strTemp->Get();
                        }
                    } else  // NOT abbreviated. (Full box receipt is already
                            // loaded.)
                    {
                        auto pPayment = GetInstrumentByReceiptID(
                            *pNym, lReceiptId, *pInbox);
                        if (false == bool(pPayment))  // then we treat it like
                                                      // it's abbreviated.
                        {
                            str_type =
                                pBoxTrans
                                    ->GetTypeString();  // instrumentNotice,
                                                        // etc.
                            Amount lAmount =
                                pBoxTrans->GetAbbrevDisplayAmount();

                            if (0 == lAmount)
                                lAmount = pBoxTrans->GetReceiptAmount();
                            if (0 != lAmount) {
                                auto strTemp = String::Factory();
                                strTemp->Format("%" PRId64 "", lAmount);
                                str_amount = strTemp->Get();
                            }
                        }
                        // We have pPayment, the instrument accompanying the
                        // receipt in the payments inbox.
                        //
                        else if (pPayment->SetTempValues()) {
                            TransactionNumber lPaymentInstrumentTransNum = 0;
                            TransactionNumber lOpeningNum = 0;
                            // It may be that the Nym has a transaction number
                            // of his own
                            // on this instrument, even if it's not the main
                            // transaction
                            // number of the instrument. So we check for that
                            // first. Otherwise
                            // we just grab the main transaction number for the
                            // instrument,
                            // regardless of which Nym it belongs to. (It will
                            // be used "for display"
                            // purposes only.)
                            //
                            // UPDATE: The point of having a transaction number
                            // "For Display"
                            // is so Alice can look in her outbox, and Bob can
                            // look in his inbox,
                            // and they can each find the same transaction based
                            // on the same
                            // transaction number that's displayed.
                            //
                            // THEREFORE, it's much better to have a number they
                            // both agree
                            // is used "for display" versus having them both use
                            // their own
                            // personal numbers, which are useless for matching
                            // up to other
                            // users.
                            //
                            if (pPayment->GetOpeningNum(lOpeningNum, theNymID))
                                lPaymentInstrumentTransNum = lOpeningNum;
                            else
                                pPayment->GetTransactionNum(
                                    lPaymentInstrumentTransNum);

                            pPayment->GetTransNumDisplay(
                                lPaymentInstrumentTransNumDisplay);

                            if (lPaymentInstrumentTransNumDisplay <= 0)
                                lPaymentInstrumentTransNumDisplay =
                                    lPaymentInstrumentTransNum;

                            pPayment->GetValidFrom(tValidFrom);
                            pPayment->GetValidTo(tValidTo);

                            if (tValidFrom > OT_TIME_ZERO) {
                                const std::uint64_t lFrom =
                                    OTTimeGetSecondsFromTime(tValidFrom);
                                auto strFrom = String::Factory();
                                strFrom->Format("%" PRIu64 "", lFrom);
                                str_date = strFrom->Get();
                            }
                            auto strMemo = String::Factory();
                            if (pPayment->GetMemo(strMemo)) {
                                str_memo = strMemo->Get();
                            }

                            auto paymentNotaryId = Identifier::Factory();
                            bool bGotPaymentNotary =
                                pPayment->GetNotaryID(paymentNotaryId);

                            if (bGotPaymentNotary) {
                                str_payment_notary = paymentNotaryId->str();
                            }

                            pPayment->GetPaymentContents(strContents);
                            auto theInstrumentDefinitionID =
                                     Identifier::Factory(),
                                 theSenderAcctID = Identifier::Factory();

                            if (pPayment->GetInstrumentDefinitionID(
                                    theInstrumentDefinitionID)) {
                                auto strTemp =
                                    String::Factory(theInstrumentDefinitionID);
                                const std::string str_inpmt_asset(
                                    strTemp->Get());  // The instrument
                                                      // definition we found
                                // on the payment (if we found anything.)
                                auto it_asset = m_assets.find(str_inpmt_asset);
                                if (it_asset != m_assets.end())  // Found it on
                                                                 // the map of
                                                                 // instrument
                                // definitions we care about.
                                {
                                    p_str_asset_type =
                                        &(it_asset->first);  // Set the asset
                                                             // type ID.
                                    p_str_asset_name = &(
                                        it_asset->second);  // The CurrencyTLA.
                                                            // Examples: USD,
                                                            // BTC, etc.
                                } else {
                                    // There was definitely an instrument
                                    // definition on the
                                    // instrument, and it definitely
                                    // did not match any of the assets that we
                                    // care about.
                                    // Therefore, skip.
                                    //
                                    otErr << __FUNCTION__
                                          << ": Skipping: Incoming payment (we "
                                             "don't care about asset "
                                          << str_inpmt_asset.c_str() << ")\n";
                                    continue;
                                }
                            }
                            if (str_sender_acct_id.empty() &&
                                pPayment->GetSenderAcctIDForDisplay(
                                    theSenderAcctID)) {
                                auto strTemp = String::Factory(theSenderAcctID);
                                str_sender_acct_id = strTemp->Get();
                            }
                            // By this point, p_str_asset_type and
                            // p_str_asset_name are definitely set.
                            OT_ASSERT(nullptr != p_str_asset_type);  // and it's
                                                                     // either
                            // blank, or
                            // it's one of
                            // the asset
                            // types we
                            // care about.
                            OT_ASSERT(nullptr != p_str_asset_name);  // and it's
                                                                     // either
                            // blank, or
                            // it's one of
                            // the asset
                            // types we
                            // care about.
                            // Instrument type (cheque, voucher, etc)
                            std::int32_t nType =
                                static_cast<std::int32_t>(pPayment->GetType());

                            str_type = GetTypeString(nType);
                            Amount lAmount = 0;

                            if (pPayment->GetAmount(lAmount)) {
                                auto strTemp = String::Factory();
                                strTemp->Format("%" PRId64 "", lAmount);
                                str_amount = strTemp->Get();
                            }
                        }
                    }
                    LogVerbose(OT_METHOD)(__FUNCTION__)(
                        ": ADDED: pending incoming payment (str_type: ")(
                        str_type.c_str())(").")
                        .Flush();

                    shared_ptr_Record sp_Record(new Record(
                        *this,
                        str_transport_notary,
                        str_payment_notary,
                        *p_str_asset_type,
                        *p_str_asset_name,
                        str_nym_id,  // This is the Nym WHOSE BOX IT IS.
                        RecordList::s_blank,  // This is the Nym's account
                                              // for
                        // this box. (Blank for payments
                        // inbox.)
                        // Everything above this line, it stores a reference
                        // to
                        // an external string.
                        // Everything below this line, it makes its own
                        // internal
                        // copy of the string.
                        str_name,  // name of sender (since its in the
                                   // inbox.)
                        str_date,  // the "valid from" date on the
                                   // instrument.
                        str_amount,
                        str_type,  // pending, chequeReceipt, etc.
                        true,      // I believe all incoming "payment inbox"
                        // items are pending. (Cheques waiting to be cashed,
                        // smart contracts waiting to be signed, etc.)
                        false,  // bIsOutgoing=false. (Since this is the
                                // payment
                                // INbox, nothing is outgoing...)
                        false,  // bIsRecord
                        false,  // bIsReceipt
                        Record::Instrument));
                    if (strContents->Exists())
                        sp_Record->SetContents(strContents->Get());
                    sp_Record->SetDateRange(tValidFrom, tValidTo);
                    sp_Record->SetBoxIndex(nIndex);
                    if (!str_memo.empty()) sp_Record->SetMemo(str_memo);
                    if (!str_sender_nym_id.empty())
                        sp_Record->SetOtherNymID(str_sender_nym_id);
                    if (!str_sender_acct_id.empty())
                        sp_Record->SetOtherAccountID(str_sender_acct_id);

                    if (lPaymentInstrumentTransNumDisplay > 0)
                        sp_Record->SetTransNumForDisplay(
                            lPaymentInstrumentTransNumDisplay);
                    else
                        sp_Record->SetTransNumForDisplay(
                            pBoxTrans->GetReferenceNumForDisplay());
                    sp_Record->SetTransactionNum(
                        pBoxTrans->GetTransactionNum());

                    //                    otErr << "DEBUGGING! Added pending
                    //                    incoming payment. str_type: " <<
                    //                    str_type.c_str() <<
                    //                    "\n pBoxTrans->GetTransactionNum(): "
                    //                    << pBoxTrans->GetTransactionNum() <<
                    //                    "\n
                    //                    pBoxTrans->GetReferenceNumForDisplay()"
                    //                    <<
                    //                    pBoxTrans->GetReferenceNumForDisplay()
                    //                    << "\n";

                    m_contents.push_back(sp_Record);

                }  // looping through inbox.
            } else
                LogDetail(OT_METHOD)(__FUNCTION__)(
                    ": Failed loading payments inbox. "
                    "(Probably just doesn't exist yet).")
                    .Flush();
            nIndex = (-1);

            // Also loop through its record box. For this record box, pass the
            // NYM_ID twice, since it's the recordbox for the Nym.
            // OPTIMIZE FYI: m_bRunFast impacts run speed here.
            std::unique_ptr<Ledger> pRecordbox{nullptr};

            if (false == theNymID->empty()) {
                pRecordbox =
                    m_bRunFast
                        ? Opentxs::Client().OTAPI().LoadRecordBoxNoVerify(
                              theMsgNotaryID, theNymID, theNymID)  // twice.
                        : Opentxs::Client().OTAPI().LoadRecordBox(
                              theMsgNotaryID, theNymID, theNymID);
            }

            // It loaded up, so let's loop through it.
            if (false != bool(pRecordbox)) {
                for (auto& it : pRecordbox->GetTransactionMap()) {
                    const TransactionNumber lReceiptId = it.first;
                    auto pBoxTrans = it.second;
                    OT_ASSERT(false != bool(pBoxTrans));
                    bool bOutgoing = false;

                    originType theOriginType = pBoxTrans->GetOriginType();

                    // Let's say Alice sends a payment plan to Bob, and then Bob
                    // activates it. Alice will receive a notice, via her
                    // Nymbox,
                    // which will be placed in her Nym record box. (The pending
                    // outgoing
                    // plan is removed, since the notice means it's no longer
                    // "pending"
                    // but instead, now officially activated or canceled.) My
                    // point? The
                    // notice in her record box relates to her SENT plan, not a
                    // received
                    // plan. It needs to show up as outgoing/sent, NOT
                    // incoming/received.
                    //
                    // UPDATE: The above is true, but not in the case of Bob! He
                    // had a "pending"
                    // in his inbox, so that needs to become an "activated" in
                    // his inbox! (Not outbox.)
                    //
                    //                  if (transactionType::notice ==
                    //                  pBoxTrans->GetType())
                    //                     bOutgoing = true;
                    // -------------------------------------------
                    bool bHasSuccess = false;
                    bool bIsSuccess = false;

                    bool bCanceled = false;
                    // (A transaction object containing a notice of a success is
                    // not the same thing
                    // as a transaction object containing a successful
                    // transaction. In the latter
                    // case, it's the transaction itself. In the former, it's a
                    // separate object that
                    // serves notice that something has occurred. A notice, for
                    // example, does not
                    // contain a balance agreement. So you couldn't say it's a
                    // "successful transaction"
                    // even though it is serving notice to various parties that
                    // a cron item was successfully
                    // (or not) activated.)
                    // -------------------------------------------
                    ++nIndex;  // 0 on first iteration.
                    LogVerbose(OT_METHOD)(__FUNCTION__)(
                        ": Payment RECORD index: ")(nIndex)(".")
                        .Flush();
                    std::string str_name;  // name of sender OR recipient
                    // (depending on whether it was originally incoming or
                    // outgoing.)
                    std::string str_other_nym_id;
                    std::string str_other_acct_id;

                    Record::RecordType recordType = Record::Instrument;

                    if (!pBoxTrans->IsAbbreviated()) {
                        // We ignore the return value of GetSuccess, since it's
                        // not
                        // detailed enough for our needs here.
                        //
                        pBoxTrans->GetSuccess(&bHasSuccess, &bIsSuccess);

                        if (transactionType::notice == pBoxTrans->GetType()) {
                            recordType = Record::Notice;

                            if (pBoxTrans->IsCancelled()) bCanceled = true;
                        }
                        // ----------------------------------------
                        auto theSenderID = Identifier::Factory(),
                             theSenderAcctID = Identifier::Factory();
                        auto theRecipientID = Identifier::Factory(),
                             theRecipientAcctID = Identifier::Factory();

                        if (pBoxTrans->GetSenderNymIDForDisplay(theSenderID)) {
                            const auto strSenderID =
                                String::Factory(theSenderID);
                            const std::string str_sender_id(strSenderID->Get());

                            // Usually, Nym is the RECIPIENT. Sometimes he's the
                            // sender.
                            // Either way, we want the OTHER ID (the other Nym)
                            // for display.
                            // So here, if Nym's CLEARLY the sender, then we
                            // want the RECIPIENT.
                            // Whereas if Nym were the recipient, then we'd want
                            // the SENDER. (For display.)
                            //
                            if (0 == str_nym_id.compare(
                                         str_sender_id))  // str_nym_id IS
                                                          // str_sender_id.
                                                          // (Therefore we want
                                                          // recipient.)
                            {
                                if (transactionType::notice ==
                                    pBoxTrans->GetType()) {
                                    if (originType::origin_payment_plan ==
                                        theOriginType)
                                        bOutgoing =
                                            false;  // Payment Plan "sender"
                                                    // of funds (payer) is
                                                    // the recipient of the
                                                    // plan.
                                    if (originType::origin_smart_contract ==
                                        theOriginType)
                                        bOutgoing =
                                            true;  // TODO: smart contracts?
                                } else
                                    bOutgoing = true;  // if Nym is the sender,
                                                       // then it must have been
                                                       // outgoing.

                                if (pBoxTrans->GetRecipientNymIDForDisplay(
                                        theRecipientID)) {
                                    const auto strRecipientID =
                                        String::Factory(theRecipientID);
                                    const std::string str_recipient_id(
                                        strRecipientID->Get());

                                    auto strNameTemp = String::Factory();
                                    strNameTemp->Format(
                                        RecordList::textTo(),
                                        str_recipient_id.c_str());
                                    str_name = strNameTemp->Get();
                                    str_other_nym_id = str_recipient_id;

                                    if (pBoxTrans->GetRecipientAcctIDForDisplay(
                                            theRecipientAcctID)) {
                                        const auto strRecipientAcctID =
                                            String::Factory(theRecipientAcctID);
                                        str_other_acct_id =
                                            strRecipientAcctID->Get();
                                    }
                                }
                            } else  // str_nym_id IS NOT str_sender_id.
                                    // (Therefore we want sender.)
                            {  // In this case, some OTHER Nym is the sender, so
                                // it must have been incoming.
                                // (And bOutgoing is already false.)

                                if (transactionType::notice ==
                                    pBoxTrans->GetType()) {
                                    if (originType::origin_payment_plan ==
                                        theOriginType)
                                        bOutgoing =
                                            true;  // Payment Plan "recipient"
                                                   // of funds (merchant) is the
                                                   // sender of the plan.
                                    if (originType::origin_smart_contract ==
                                        theOriginType)
                                        bOutgoing =
                                            false;  // TODO: Smart contracts?
                                } else
                                    bOutgoing = false;

                                auto strNameTemp = String::Factory();
                                strNameTemp->Format(
                                    RecordList::textFrom(),
                                    str_sender_id.c_str());
                                str_name = strNameTemp->Get();
                                str_other_nym_id = str_sender_id;

                                if (pBoxTrans->GetSenderAcctIDForDisplay(
                                        theSenderAcctID)) {
                                    const auto strSenderAcctID =
                                        String::Factory(theSenderAcctID);
                                    str_other_acct_id = strSenderAcctID->Get();
                                }
                            }
                        }
                        // In this block below, we already KNOW
                        // GetSenderNymIDForDisplay is EMPTY.
                        // (So it's "recipient or bust.")
                        else if (pBoxTrans->GetRecipientNymIDForDisplay(
                                     theRecipientID)) {
                            const auto strRecipientID =
                                String::Factory(theRecipientID);
                            const std::string str_recipient_id(
                                strRecipientID->Get());

                            if (0 !=
                                str_nym_id.compare(
                                    str_recipient_id))  // str_nym_id is NOT
                                                        // str_recipient_id.
                                                        // (Therefore we want
                                                        // str_recipient_id.)
                            {
                                // If Nym is not the recipient, then he must be
                                // the sender.
                                // (Therefore it must be outgoing.)
                                //
                                if (transactionType::notice ==
                                    pBoxTrans->GetType())
                                    bOutgoing = false;  // Payment Plan "sender"
                                                        // of funds (payer) is
                                                        // the recipient of the
                                                        // plan.
                                // TODO: Smart contracts?
                                else
                                    bOutgoing = true;  // if Nym is the sender,
                                                       // then it must have been
                                                       // outgoing.

                                auto strNameTemp = String::Factory();
                                strNameTemp->Format(
                                    RecordList::textTo(),
                                    str_recipient_id.c_str());
                                str_name = strNameTemp->Get();
                                str_other_nym_id = str_recipient_id;
                                if (pBoxTrans->GetRecipientAcctIDForDisplay(
                                        theRecipientAcctID)) {
                                    const auto strRecipientAcctID =
                                        String::Factory(theRecipientAcctID);
                                    str_other_acct_id =
                                        strRecipientAcctID->Get();
                                }
                            }
                        }
                    }  // if not abbreviated.
                    time64_t tValidFrom = OT_TIME_ZERO, tValidTo = OT_TIME_ZERO;
                    std::string str_date =
                        "0";  // the "date signed" on the transaction receipt.
                    time64_t tDateSigned = pBoxTrans->GetDateSigned();

                    if (tDateSigned > OT_TIME_ZERO) {
                        tValidFrom = tDateSigned;
                        const std::uint64_t lDateSigned =
                            OTTimeGetSecondsFromTime(tDateSigned);
                        auto strDateSigned = String::Factory();
                        strDateSigned->Format("%" PRIu64 "", lDateSigned);
                        str_date = strDateSigned->Get();
                    }
                    const std::string* p_str_asset_type =
                        &RecordList::s_blank;  // <========== ASSET TYPE
                    const std::string* p_str_asset_name =
                        &RecordList::s_blank;  // instrument definition
                                               // display name.
                    const std::string* p_str_account =
                        &RecordList::s_blank;  // <========== ACCOUNT
                    std::string str_amount;    // <========== AMOUNT
                    std::string str_type;      // Instrument type.
                    std::string str_memo;  // Instrument memo (if applicable.)
                    auto strContents =
                        String::Factory();  // Instrument contents.

                    std::string str_payment_notary_id;
                    TransactionNumber lPaymentInstrumentTransNumDisplay = 0;

                    if (pBoxTrans->IsAbbreviated()) {
                        str_type =
                            pBoxTrans->GetTypeString();  // instrumentNotice,
                                                         // etc.
                        Amount lAmount = pBoxTrans->GetAbbrevDisplayAmount();

                        if (0 != lAmount) {
                            auto strTemp = String::Factory();
                            strTemp->Format("%" PRId64 "", lAmount);
                            str_amount = strTemp->Get();
                        }
                    } else  // NOT abbreviated. (Full box receipt is already
                            // loaded.)
                    {
                        auto pPayment = GetInstrumentByReceiptID(
                            *pNym, lReceiptId, *pRecordbox);

                        if (false == bool(pPayment))  // then we treat it like
                                                      // it's abbreviated.
                        {
                            str_type =
                                pBoxTrans
                                    ->GetTypeString();  // instrumentNotice,
                                                        // etc.
                            Amount lAmount =
                                pBoxTrans->GetAbbrevDisplayAmount();

                            if (0 != lAmount) {
                                auto strTemp = String::Factory();
                                strTemp->Format("%" PRId64 "", lAmount);
                                str_amount = strTemp->Get();
                            }
                        }
                        // We have pPayment, the instrument accompanying the
                        // receipt in the payments recordbox.
                        else if (pPayment->SetTempValues()) {
                            TransactionNumber lPaymentInstrumentTransNum = 0;
                            TransactionNumber lOpeningNum = 0;

                            if (pPayment->GetOpeningNum(lOpeningNum, theNymID))
                                lPaymentInstrumentTransNum = lOpeningNum;
                            else
                                pPayment->GetTransactionNum(
                                    lPaymentInstrumentTransNum);

                            pPayment->GetTransNumDisplay(
                                lPaymentInstrumentTransNumDisplay);

                            if (lPaymentInstrumentTransNumDisplay <= 0)
                                lPaymentInstrumentTransNumDisplay =
                                    lPaymentInstrumentTransNum;

                            pPayment->GetValidFrom(tValidFrom);
                            pPayment->GetValidTo(tValidTo);

                            if (tValidFrom > OT_TIME_ZERO) {
                                const std::uint64_t lFrom =
                                    OTTimeGetSecondsFromTime(tValidFrom);
                                auto strFrom = String::Factory();
                                strFrom->Format("%" PRIu64 "", lFrom);
                                str_date = strFrom->Get();
                            }
                            pPayment->GetPaymentContents(strContents);
                            auto theAccountID = Identifier::Factory();

                            if (bOutgoing)  // Nym is sender.
                            {
                                if (((pPayment->IsPaymentPlan() ||
                                      pPayment->IsNotice()) &&
                                     pPayment->GetRecipientAcctID(
                                         theAccountID)) ||
                                    pPayment->GetSenderAcctIDForDisplay(
                                        theAccountID))  // Since Nym is ME, the
                                                        // Account must be MY
                                                        // acct.
                                {  // (If this record was originally OUTgoing,
                                    // then the SENDER's account is MY acct.)
                                    //
                                    // UPDATE: Unless I sent a payment plan
                                    // proposal. In which case I, the merchant,
                                    // am the "sender" of the proposal, but the
                                    // "sender" on the instrument is the
                                    // customer,
                                    // since he will be "sending" me the money!
                                    // So the "Recipient" of the proposal
                                    // message
                                    // (outgoing from my perspective) is
                                    // actually the "Sender" of the money
                                    // according to the
                                    // instrument (which will be incoming,
                                    // according to my perspective, when those
                                    // payments
                                    // start to arrive.) Thus, though the
                                    // merchant is the "sender" of the proposal,
                                    // he's the
                                    // "recipient" on the instrument.
                                    auto strTemp =
                                        String::Factory(theAccountID);
                                    std::string str_outpmt_account =
                                        strTemp->Get();  // The accountID we
                                                         // found on the payment
                                                         // (only applies to
                                                         // outgoing payments.)
                                    auto it_acct = std::find(
                                        m_accounts.begin(),
                                        m_accounts.end(),
                                        str_outpmt_account);
                                    if (it_acct !=
                                        m_accounts.end())  // Found it on the
                                                           // list of accounts
                                                           // we care about.
                                    {
                                        p_str_account = &(*it_acct);
                                    } else {
                                        // There was definitely an account on
                                        // the instrument, and it definitely
                                        // did not match any of the accounts
                                        // that we care about. Therefore, skip.
                                        //
                                        LogVerbose(OT_METHOD)(__FUNCTION__)(
                                            ": Skipping 'sent payment' "
                                            "record. (We don't care "
                                            "about account ")(
                                            str_outpmt_account.c_str())(").")
                                            .Flush();
                                        continue;
                                    }
                                }
                            } else  // Nym is recipient.
                            {
                                // Why is this here? Because if Nym is
                                // recipient, let's say he received
                                // an instrumentNotice containing a
                                // sendNymInstrument message containing
                                // an incoming cheque. Well... that incoming
                                // cheque (the payload on
                                // sendNymInstrument message) is ENCRYPTED.
                                // Meaning the above calls to
                                // pBoxReceipt->GetSenderAcctID on the
                                // instrumentNotice transaction will
                                // FAIL. One option is to pass pNym into
                                // GetSenderAcctID so it can
                                // decrypt the payload and return the value. But
                                // since we already have
                                // the payload decrypted here (we already have
                                // the cheque loaded up
                                // here) we can just grab the senderAcctID
                                // directly from the cheque.
                                // That's why this is here -- because this is
                                // where we KNOW we have the
                                // account ID -- so we grab it.
                                //
                                if (str_other_acct_id.empty() &&
                                    (((pPayment->IsPaymentPlan() ||
                                       pPayment->IsNotice()) &&
                                      pPayment->GetRecipientAcctID(
                                          theAccountID)) ||
                                     pPayment->GetSenderAcctIDForDisplay(
                                         theAccountID))) {
                                    auto strTemp =
                                        String::Factory(theAccountID);
                                    str_other_acct_id = strTemp->Get();
                                }
                            }
                            // By this point, p_str_account is definitely set.
                            OT_ASSERT(nullptr != p_str_account);  // and it's
                                                                  // either
                                                                  // blank, or
                                                                  // it's one of
                                                                  // the
                                                                  // accounts we
                                                                  // care about.
                            auto theInstrumentDefinitionID =
                                Identifier::Factory();

                            if (pPayment->GetInstrumentDefinitionID(
                                    theInstrumentDefinitionID)) {
                                auto strTemp =
                                    String::Factory(theInstrumentDefinitionID);
                                const std::string str_inpmt_asset(
                                    strTemp->Get());  // The instrument
                                                      // definition we found on
                                                      // the payment (if we
                                                      // found anything.)
                                auto it_asset = m_assets.find(str_inpmt_asset);
                                if (it_asset !=
                                    m_assets.end())  // Found it on the map of
                                                     // instrument definitions
                                                     // we care about.
                                {
                                    p_str_asset_type =
                                        &(it_asset->first);  // Set the asset
                                                             // type ID.
                                    p_str_asset_name =
                                        &(it_asset->second);  // The
                                                              // CurrencyTLA.
                                                              // Examples: USD,
                                                              // BTC, etc.
                                } else {
                                    // There was definitely an instrument
                                    // definition on the instrument, and it
                                    // definitely
                                    // did not match any of the assets that we
                                    // care about. Therefore, skip.
                                    //
                                    otErr << __FUNCTION__
                                          << ": Skipping: Payment record (we "
                                             "don't care about instrument "
                                             "definition "
                                          << str_inpmt_asset.c_str() << ")\n";
                                    continue;
                                }
                            }
                            // By this point, p_str_asset_type and
                            // p_str_asset_name are definitely set.
                            OT_ASSERT(nullptr != p_str_asset_type);  // and it's
                            // either blank,
                            // or it's one of
                            // the asset
                            // types we care
                            // about.
                            OT_ASSERT(nullptr != p_str_asset_name);  // and it's
                            // either blank,
                            // or it's one of
                            // the asset
                            // types we care
                            // about.

                            auto strMemo = String::Factory();
                            if (pPayment->GetMemo(strMemo)) {
                                str_memo = strMemo->Get();
                            }
                            // Instrument type (cheque, voucher, etc)
                            std::int32_t nType =
                                static_cast<std::int32_t>(pPayment->GetType());

                            str_type = GetTypeString(nType);
                            Amount lAmount = 0;

                            if (pPayment->GetAmount(lAmount)) {
                                auto strTemp = String::Factory();
                                strTemp->Format("%" PRId64 "", lAmount);
                                str_amount = strTemp->Get();
                            }

                            auto paymentNotaryId = Identifier::Factory();
                            if (pPayment->GetNotaryID(paymentNotaryId)) {
                                str_payment_notary_id = paymentNotaryId->str();
                            }
                        }
                    }
                    LogVerbose(OT_METHOD)(__FUNCTION__)(
                        ": ADDED: Payment record ")(
                        bOutgoing ? "(sent)" : "(received)")(" (str_type: ")(
                        str_type.c_str())(").")
                        .Flush();

                    shared_ptr_Record sp_Record(new Record(
                        *this,
                        theMsgNotaryID->str(),  // Transport Notary
                        str_payment_notary_id,  // Payment Notary
                        *p_str_asset_type,
                        *p_str_asset_name,
                        str_nym_id,      // This is the Nym WHOSE BOX IT IS.
                        *p_str_account,  // This is the Nym's account for
                                         // this box. (Blank for incoming,
                                         // set for outgoing.)
                        // Everything above this line, it stores a reference
                        // to an external string.
                        // Everything below this line, it makes its own
                        // internal copy of the string.
                        str_name,  // name of sender or recipient (since its
                                   // in the recordbox.)
                        str_date,  // the "date signed" on the receipt.
                        str_amount,
                        str_type,   // pending, chequeReceipt, etc.
                        false,      // Everything in the recordbox is finished.
                                    // (NOT pending.)
                        bOutgoing,  // Since it's the recordbox, it contains
                                    // both incoming and outgoing receipts.
                        true,       // IsRecord
                        false,      // IsReceipt,
                        recordType));

                    if (strContents->Exists())
                        sp_Record->SetContents(strContents->Get());

                    sp_Record->SetDateRange(tValidFrom, tValidTo);
                    sp_Record->SetBoxIndex(nIndex);

                    if (!str_memo.empty()) sp_Record->SetMemo(str_memo);
                    if (!str_other_nym_id.empty())
                        sp_Record->SetOtherNymID(str_other_nym_id);
                    if (!str_other_acct_id.empty())
                        sp_Record->SetOtherAccountID(str_other_acct_id);

                    if (lPaymentInstrumentTransNumDisplay > 0)
                        sp_Record->SetTransNumForDisplay(
                            lPaymentInstrumentTransNumDisplay);
                    else
                        sp_Record->SetTransNumForDisplay(
                            pBoxTrans->GetReferenceNumForDisplay());
                    sp_Record->SetTransactionNum(
                        pBoxTrans->GetTransactionNum());

                    if (bHasSuccess) sp_Record->SetSuccess(bIsSuccess);

                    if (bCanceled) sp_Record->SetCanceled();

                    sp_Record->SetOriginType(theOriginType);

                    //                    otErr << "DEBUGGING! RECORD LIST:
                    //                    Added " << (bOutgoing ? "sent":
                    //                    "received") << " payment record: " <<
                    //                    pBoxTrans->GetTypeString() << "\n
                    //                    str_nym_id: " << str_nym_id << "\n
                    //                    str_other_nym_id: " <<
                    //                    str_other_nym_id <<
                    //                    "\n pBoxTrans->GetTransactionNum(): "
                    //                    << pBoxTrans->GetTransactionNum() <<
                    //                    "\n
                    //                    pBoxTrans->GetReferenceNumForDisplay()"
                    //                    <<
                    //                    pBoxTrans->GetReferenceNumForDisplay()
                    //                    << "\n";

                    m_contents.push_back(sp_Record);

                }  // Loop through Recordbox
            } else
                LogDetail(OT_METHOD)(__FUNCTION__)(
                    ": Failed loading payments record "
                    "box. (Probably just doesn't exist yet).")
                    .Flush();

            // EXPIRED RECORDS:
            nIndex = (-1);

            // Also loop through its expired record box.
            // OPTIMIZE FYI: m_bRunFast impacts run speed here.
            std::unique_ptr<Ledger> pExpiredbox{nullptr};

            if (false == theNymID->empty()) {
                pExpiredbox =
                    m_bRunFast
                        ? Opentxs::Client().OTAPI().LoadExpiredBoxNoVerify(
                              theMsgNotaryID, theNymID)
                        : Opentxs::Client().OTAPI().LoadExpiredBox(
                              theMsgNotaryID, theNymID);
            }

            // It loaded up, so let's loop through it.
            if (pExpiredbox) {
                for (auto& it : pExpiredbox->GetTransactionMap()) {
                    const TransactionNumber lReceiptId = it.first;
                    auto pBoxTrans = it.second;
                    OT_ASSERT(false != bool(pBoxTrans));
                    bool bOutgoing{false};

                    // Let's say Alice sends a payment plan to Bob, and then Bob
                    // activates it. Alice will receive a notice, via her
                    // Nymbox,
                    // which will be placed in her Nym record box. (The pending
                    // outgoing
                    // plan is removed, since the notice means it's no longer
                    // "pending" but instead, now officially activated or
                    // canceled.) My point? The notice in her record box relates
                    // to her SENT plan, not a received plan. It needs to show
                    // up as outgoing/sent, NOT incoming/received.
                    //
                    //                  if (transactionType::notice ==
                    //                  pBoxTrans->GetType())
                    //                      bOutgoing = true;
                    // -------------------------------------------
                    bool bHasSuccess{false};
                    bool bIsSuccess{false};

                    bool bCanceled{false};
                    // (A transaction object containing a notice of a success is
                    // not the same thing as a transaction object containing a
                    // successful transaction. In the latter case, it's the
                    // transaction itself. In the former, it's a separate object
                    // that serves notice that something has occurred. A notice,
                    // for example, does not contain a balance agreement. So you
                    // couldn't say it's a "successful transaction" even though
                    // it is serving notice to various parties that a cron item
                    // was successfully (or not) activated.)
                    //
                    // -------------------------------------------
                    ++nIndex;  // 0 on first iteration.
                    LogVerbose(OT_METHOD)(__FUNCTION__)(
                        ": Expired payment RECORD index: ")(nIndex)(".")
                        .Flush();
                    std::string str_name;  // name of sender OR recipient
                                           // (depending on whether it was
                                           // originally incoming or outgoing.)
                    std::string str_other_nym_id;
                    std::string str_other_acct_id;

                    Record::RecordType recordType = Record::Instrument;

                    if (!pBoxTrans->IsAbbreviated()) {
                        // We ignore the return value of GetSuccess, since it's
                        // not detailed enough for our needs here.
                        //
                        pBoxTrans->GetSuccess(&bHasSuccess, &bIsSuccess);

                        if (transactionType::notice == pBoxTrans->GetType()) {
                            recordType = Record::Notice;

                            if (pBoxTrans->IsCancelled()) bCanceled = true;
                        }
                        // ----------------------------------------
                        auto theSenderID = Identifier::Factory(),
                             theSenderAcctID = Identifier::Factory();
                        auto theRecipientID = Identifier::Factory(),
                             theRecipientAcctID = Identifier::Factory();

                        if (pBoxTrans->GetSenderNymIDForDisplay(theSenderID)) {
                            const auto strSenderID =
                                String::Factory(theSenderID);
                            const std::string str_sender_id(strSenderID->Get());

                            // Usually, Nym is the RECIPIENT. Sometimes he's the
                            // sender.
                            // Either way, we want the OTHER ID (the other Nym)
                            // for display.
                            // So here, if Nym's CLEARLY the sender, then we
                            // want the RECIPIENT.
                            // Whereas if Nym were the recipient, then we'd want
                            // the SENDER. (For display.)
                            //
                            if (0 == str_nym_id.compare(
                                         str_sender_id))  // str_nym_id IS
                                                          // str_sender_id.
                                                          // (Therefore we want
                                                          // recipient.)
                            {
                                if (transactionType::notice ==
                                    pBoxTrans->GetType())
                                    bOutgoing = false;  // Payment Plan "sender"
                                                        // of funds (payer) is
                                                        // the recipient of the
                                                        // plan.
                                // TODO: Smart contracts?
                                else
                                    bOutgoing = true;  // if Nym is the sender,
                                                       // then it must have been
                                                       // outgoing.

                                if (pBoxTrans->GetRecipientNymIDForDisplay(
                                        theRecipientID)) {
                                    const auto strRecipientID =
                                        String::Factory(theRecipientID);
                                    const std::string str_recipient_id(
                                        strRecipientID->Get());

                                    auto strNameTemp = String::Factory();
                                    strNameTemp->Format(
                                        RecordList::textTo(),
                                        str_recipient_id.c_str());
                                    str_name = strNameTemp->Get();
                                    str_other_nym_id = str_recipient_id;

                                    if (pBoxTrans->GetRecipientAcctIDForDisplay(
                                            theRecipientAcctID)) {
                                        const auto strRecipientAcctID =
                                            String::Factory(theRecipientAcctID);
                                        str_other_acct_id =
                                            strRecipientAcctID->Get();
                                    }
                                }
                            } else  // str_nym_id IS NOT str_sender_id.
                                    // (Therefore we want sender.)
                            {  // In this case, some OTHER Nym is the sender, so
                                // it must have been incoming.
                                // (And bOutgoing is already false.)

                                if (transactionType::notice ==
                                    pBoxTrans->GetType())
                                    bOutgoing = true;  // Payment Plan
                                                       // "recipient" of funds
                                                       // (merchant) is the
                                                       // sender of the plan.
                                                       // TODO: Smart contracts?
                                else
                                    bOutgoing = false;

                                auto strNameTemp = String::Factory();
                                strNameTemp->Format(
                                    RecordList::textFrom(),
                                    str_sender_id.c_str());
                                str_name = strNameTemp->Get();
                                str_other_nym_id = str_sender_id;

                                if (pBoxTrans->GetSenderAcctIDForDisplay(
                                        theSenderAcctID)) {
                                    const auto strSenderAcctID =
                                        String::Factory(theSenderAcctID);
                                    str_other_acct_id = strSenderAcctID->Get();
                                }
                            }
                        }
                        // In this block below, we already KNOW
                        // GetSenderNymIDForDisplay is EMPTY.
                        // (So it's "recipient or bust.")
                        else if (pBoxTrans->GetRecipientNymIDForDisplay(
                                     theRecipientID)) {
                            const auto strRecipientID =
                                String::Factory(theRecipientID);
                            const std::string str_recipient_id(
                                strRecipientID->Get());

                            if (0 !=
                                str_nym_id.compare(
                                    str_recipient_id))  // str_nym_id is NOT
                                                        // str_recipient_id.
                            {  // (Therefore we want str_recipient_id.)
                                // If Nym is not the recipient, then he must be
                                // the sender.
                                // (Therefore it must be outgoing.)

                                if (transactionType::notice ==
                                    pBoxTrans->GetType())
                                    bOutgoing = false;  // Payment Plan "sender"
                                                        // of funds (payer) is
                                                        // the recipient of the
                                                        // plan.
                                // TODO: Smart contracts?
                                else
                                    bOutgoing = true;  // if Nym is the sender,
                                                       // then it must have been
                                                       // outgoing.

                                auto strNameTemp = String::Factory();
                                strNameTemp->Format(
                                    RecordList::textTo(),
                                    str_recipient_id.c_str());
                                str_name = strNameTemp->Get();
                                str_other_nym_id = str_recipient_id;

                                if (pBoxTrans->GetRecipientAcctIDForDisplay(
                                        theRecipientAcctID)) {
                                    const auto strRecipientAcctID =
                                        String::Factory(theRecipientAcctID);
                                    str_other_acct_id =
                                        strRecipientAcctID->Get();
                                }
                            }
                        }
                    }  // if not abbreviated.
                    time64_t tValidFrom = OT_TIME_ZERO, tValidTo = OT_TIME_ZERO;
                    std::string str_date =
                        "0";  // the "date signed" on the transaction receipt.
                    time64_t tDateSigned = pBoxTrans->GetDateSigned();

                    if (tDateSigned > OT_TIME_ZERO) {
                        tValidFrom = tDateSigned;
                        const std::uint64_t lDateSigned =
                            OTTimeGetSecondsFromTime(tDateSigned);
                        auto strDateSigned = String::Factory();
                        strDateSigned->Format("%" PRIu64 "", lDateSigned);
                        str_date = strDateSigned->Get();
                    }
                    const std::string* p_str_asset_type =
                        &RecordList::s_blank;  // <========== ASSET TYPE
                    const std::string* p_str_asset_name =
                        &RecordList::s_blank;  // instrument definition
                                               // display name.
                    const std::string* p_str_account =
                        &RecordList::s_blank;  // <========== ACCOUNT
                    std::string str_amount;    // <========== AMOUNT
                    std::string str_type;      // Instrument type.
                    std::string str_memo;  // Instrument memo (if applicable.)
                    auto strContents =
                        String::Factory();  // Instrument contents.

                    std::string str_payment_notary_id;

                    TransactionNumber lPaymentInstrumentTransNumDisplay = 0;

                    if (pBoxTrans->IsAbbreviated()) {
                        str_type =
                            pBoxTrans->GetTypeString();  // instrumentNotice,
                                                         // etc.
                        Amount lAmount = pBoxTrans->GetAbbrevDisplayAmount();

                        if (0 != lAmount) {
                            auto strTemp = String::Factory();
                            strTemp->Format("%" PRId64 "", lAmount);
                            str_amount = strTemp->Get();
                        }
                    } else  // NOT abbreviated. (Full box receipt is already
                            // loaded.)
                    {
                        auto pPayment = GetInstrumentByReceiptID(
                            *pNym, lReceiptId, *pExpiredbox);

                        if (false == bool(pPayment))  // then we treat it like
                                                      // it's abbreviated.
                        {
                            str_type =
                                pBoxTrans
                                    ->GetTypeString();  // instrumentNotice,
                                                        // etc.
                            Amount lAmount =
                                pBoxTrans->GetAbbrevDisplayAmount();

                            if (0 != lAmount) {
                                auto strTemp = String::Factory();
                                strTemp->Format("%" PRId64 "", lAmount);
                                str_amount = strTemp->Get();
                            }
                        }
                        // We have pPayment, the instrument accompanying the
                        // receipt in the payments recordbox.
                        else if (pPayment->SetTempValues()) {
                            TransactionNumber lPaymentInstrumentTransNum = 0;
                            TransactionNumber lOpeningNum = 0;

                            if (pPayment->GetOpeningNum(lOpeningNum, theNymID))
                                lPaymentInstrumentTransNum = lOpeningNum;
                            else
                                pPayment->GetTransactionNum(
                                    lPaymentInstrumentTransNum);

                            pPayment->GetTransNumDisplay(
                                lPaymentInstrumentTransNumDisplay);

                            if (lPaymentInstrumentTransNumDisplay <= 0)
                                lPaymentInstrumentTransNumDisplay =
                                    lPaymentInstrumentTransNum;

                            pPayment->GetValidFrom(tValidFrom);
                            pPayment->GetValidTo(tValidTo);

                            if (tValidFrom > OT_TIME_ZERO) {
                                const std::uint64_t lFrom =
                                    OTTimeGetSecondsFromTime(tValidFrom);
                                auto strFrom = String::Factory();
                                strFrom->Format("%" PRIu64 "", lFrom);
                                str_date = strFrom->Get();
                            }
                            pPayment->GetPaymentContents(strContents);
                            auto theAccountID = Identifier::Factory();

                            auto paymentNotaryId = Identifier::Factory();
                            if (pPayment->GetNotaryID(paymentNotaryId)) {
                                str_payment_notary_id = paymentNotaryId->str();
                            }

                            if (bOutgoing)  // Nym is sender.
                            {
                                if (((pPayment->IsPaymentPlan() ||
                                      pPayment->IsNotice()) &&
                                     pPayment->GetRecipientAcctID(
                                         theAccountID)) ||
                                    pPayment->GetSenderAcctIDForDisplay(
                                        theAccountID))  // Since Nym is ME, the
                                                        // Account must be MY
                                                        // acct.
                                {  // (If this record was originally OUTgoing,
                                    // then the SENDER's account is MY acct.)
                                    // UPDATE: Unless I sent a payment plan
                                    // proposal. In which case I, the merchant,
                                    // am the "sender" of the proposal, but the
                                    // "sender" on the instrument is the
                                    // customer,
                                    // since he will be "sending" me the money!
                                    // So the "Recipient" of the proposal
                                    // message
                                    // (outgoing from my perspective) is
                                    // actually the "Sender" of the money
                                    // according to the
                                    // instrument (which will be incoming,
                                    // according to my perspective, when those
                                    // payments
                                    // start to arrive.) Thus, though the
                                    // merchant is the "sender" of the proposal,
                                    // he's the
                                    // "recipient" on the instrument.
                                    auto strTemp =
                                        String::Factory(theAccountID);
                                    std::string str_outpmt_account =
                                        strTemp->Get();  // The accountID we
                                                         // found on the payment
                                                         // (only applies to
                                                         // outgoing payments.)
                                    auto it_acct = std::find(
                                        m_accounts.begin(),
                                        m_accounts.end(),
                                        str_outpmt_account);
                                    if (it_acct !=
                                        m_accounts.end())  // Found it on the
                                                           // list of accounts
                                                           // we care about.
                                    {
                                        p_str_account = &(*it_acct);
                                    } else {
                                        // There was definitely an account on
                                        // the instrument, and it definitely
                                        // did not match any of the accounts
                                        // that we care about.
                                        // Therefore, skip.
                                        //
                                        LogVerbose(OT_METHOD)(__FUNCTION__)(
                                            ": Skipping 'sent payment' "
                                            "expired record. (We don't "
                                            "care about account ")(
                                            str_outpmt_account.c_str())(").")
                                            .Flush();
                                        continue;
                                    }
                                }
                            } else  // Nym is recipient.
                            {
                                // Why is this here? Because if Nym is
                                // recipient, let's say he received
                                // an instrumentNotice containing a
                                // sendNymInstrument message containing
                                // an incoming cheque. Well... that incoming
                                // cheque (the payload on
                                // sendNymInstrument message) is ENCRYPTED.
                                // Meaning the above calls to
                                // pBoxReceipt->GetSenderAcctID on the
                                // instrumentNotice transaction will
                                // FAIL. One option is to pass pNym into
                                // GetSenderAcctID so it can
                                // decrypt the payload and return the value. But
                                // since we already have
                                // the payload decrypted here (we already have
                                // the cheque loaded up
                                // here) we can just grab the senderAcctID
                                // directly from the cheque.
                                // That's why this is here -- because this is
                                // where we KNOW we have the
                                // account ID -- so we grab it.
                                //
                                if (str_other_acct_id.empty() &&
                                    (((pPayment->IsPaymentPlan() ||
                                       pPayment->IsNotice()) &&
                                      pPayment->GetRecipientAcctID(
                                          theAccountID)) ||
                                     pPayment->GetSenderAcctIDForDisplay(
                                         theAccountID))) {
                                    auto strTemp =
                                        String::Factory(theAccountID);
                                    str_other_acct_id = strTemp->Get();
                                }
                            }
                            // By this point, p_str_account is definitely set.
                            OT_ASSERT(nullptr != p_str_account);  // and it's
                                                                  // either
                                                                  // blank, or
                                                                  // it's one of
                                                                  // the
                                                                  // accounts we
                                                                  // care about.
                            auto theInstrumentDefinitionID =
                                Identifier::Factory();

                            if (pPayment->GetInstrumentDefinitionID(
                                    theInstrumentDefinitionID)) {
                                auto strTemp =
                                    String::Factory(theInstrumentDefinitionID);
                                const std::string str_inpmt_asset(
                                    strTemp->Get());  // The instrument
                                                      // definition we found on
                                                      // the payment (if we
                                                      // found anything.)
                                auto it_asset = m_assets.find(str_inpmt_asset);
                                if (it_asset !=
                                    m_assets.end())  // Found it on the map of
                                                     // instrument definitions
                                                     // we care about.
                                {
                                    p_str_asset_type =
                                        &(it_asset->first);  // Set the asset
                                                             // type ID.
                                    p_str_asset_name =
                                        &(it_asset->second);  // The
                                                              // CurrencyTLA.
                                                              // Examples: USD,
                                                              // BTC, etc.
                                } else {
                                    // There was definitely an instrument
                                    // definition on the instrument, and
                                    // it definitely did not match any of the
                                    // assets that we care about.
                                    // Therefore, skip.
                                    //
                                    otErr << __FUNCTION__
                                          << ": Skipping: Expired payment "
                                             "record (we don't care about "
                                             "instrument definition "
                                          << str_inpmt_asset.c_str() << ")\n";
                                    continue;
                                }
                            }
                            // By this point, p_str_asset_type and
                            // p_str_asset_name are definitely set.
                            OT_ASSERT(nullptr != p_str_asset_type);  // and it's
                            // either blank,
                            // or it's one of
                            // the asset
                            // types we care
                            // about.
                            OT_ASSERT(nullptr != p_str_asset_name);  // and it's
                            // either blank,
                            // or it's one of
                            // the asset
                            // types we care
                            // about.
                            auto strMemo = String::Factory();
                            if (pPayment->GetMemo(strMemo)) {
                                str_memo = strMemo->Get();
                            }
                            // Instrument type (cheque, voucher, etc)
                            std::int32_t nType =
                                static_cast<std::int32_t>(pPayment->GetType());

                            str_type = GetTypeString(nType);
                            Amount lAmount = 0;

                            if (pPayment->GetAmount(lAmount)) {
                                auto strTemp = String::Factory();
                                strTemp->Format("%" PRId64 "", lAmount);
                                str_amount = strTemp->Get();
                            }
                        }
                    }
                    LogVerbose(OT_METHOD)(__FUNCTION__)(
                        ": ADDED: Expired payment record ")(
                        bOutgoing ? "(sent)" : "(received)")(" (str_type: ")(
                        str_type.c_str())(").")
                        .Flush();

                    shared_ptr_Record sp_Record(new Record(
                        *this,
                        theMsgNotaryID->str(),  // Transport Notary
                        str_payment_notary_id,  // Payment Notary
                        *p_str_asset_type,
                        *p_str_asset_name,
                        str_nym_id,      // This is the Nym WHOSE BOX IT IS.
                        *p_str_account,  // This is the Nym's account for
                                         // this box. (Blank for incoming,
                                         // set for outgoing.)
                        // Everything above this line, it stores a reference
                        // to an external string.
                        // Everything below this line, it makes its own
                        // internal copy of the string.
                        str_name,  // name of sender or recipient (since its
                                   // in the recordbox.)
                        str_date,  // the "date signed" on the receipt.
                        str_amount,
                        str_type,   // pending, chequeReceipt, etc.
                        false,      // Everything in the recordbox is finished.
                                    // (NOT pending.)
                        bOutgoing,  // Since it's the recordbox, it contains
                                    // both incoming and outgoing receipts.
                        true,       // IsRecord
                        false,      // IsReceipt,
                        recordType));
                    if (strContents->Exists())
                        sp_Record->SetContents(strContents->Get());

                    sp_Record->SetDateRange(tValidFrom, tValidTo);
                    sp_Record->SetExpired();
                    sp_Record->SetBoxIndex(nIndex);

                    if (!str_memo.empty()) sp_Record->SetMemo(str_memo);
                    if (!str_other_nym_id.empty())
                        sp_Record->SetOtherNymID(str_other_nym_id);
                    if (!str_other_acct_id.empty())
                        sp_Record->SetOtherAccountID(str_other_acct_id);

                    if (lPaymentInstrumentTransNumDisplay > 0)
                        sp_Record->SetTransNumForDisplay(
                            lPaymentInstrumentTransNumDisplay);
                    else
                        sp_Record->SetTransNumForDisplay(
                            pBoxTrans->GetReferenceNumForDisplay());
                    sp_Record->SetTransactionNum(
                        pBoxTrans->GetTransactionNum());

                    if (bHasSuccess) sp_Record->SetSuccess(bIsSuccess);

                    if (bCanceled) sp_Record->SetCanceled();

                    m_contents.push_back(sp_Record);

                }  // Loop through ExpiredBox
            } else
                LogDetail(OT_METHOD)(__FUNCTION__)(
                    ": Failed loading expired payments box. "
                    "(Probably just doesn't exist yet).")
                    .Flush();

        }  // Loop through servers for each Nym.
    }      // Loop through Nyms.
       // ASSET ACCOUNT -- INBOX/OUTBOX + RECORD BOX
       // Loop through the Accounts.
       //
    LogVerbose("================ ")(__FUNCTION__)(
        ": Looping through the accounts in the wallet...")
        .Flush();
    std::int32_t nAccountIndex = -1;

    for (auto& it_acct : m_accounts) {
        ++nAccountIndex;  // (0 on first iteration.)
        // For each account, loop through its inbox, outbox, and record box.
        //
        const std::string& str_account_id(it_acct);
        const auto theAccountID = Identifier::Factory(str_account_id);
        auto account = wallet_.Account(theAccountID);

        if (false == bool(account)) {
            // This can happen if the user erases the account.
            // Therefore we just need to skip it.
            LogVerbose(OT_METHOD)(__FUNCTION__)(": Skipping an account (")(
                str_account_id.c_str())(
                ") since it has disappeared from the wallet. (Probably "
                "deleted by the user).")
                .Flush();
            continue;
        }
        const Identifier& theNymID = account.get().GetNymID();
        const Identifier& theNotaryID = account.get().GetPurportedNotaryID();
        const Identifier& theInstrumentDefinitionID =
            account.get().GetInstrumentDefinitionID();
        account.Release();
        const auto strNymID = String::Factory(theNymID);
        const auto strNotaryID = String::Factory(theNotaryID);
        const auto strInstrumentDefinitionID =
            String::Factory(theInstrumentDefinitionID);
        LogVerbose(OT_METHOD)(__FUNCTION__)(": Account: ")(nAccountIndex)(
            ", ID: ")(str_account_id.c_str())(".")
            .Flush();
        const std::string str_nym_id(strNymID->Get());
        const std::string str_notary_id(strNotaryID->Get());
        const std::string str_instrument_definition_id(
            strInstrumentDefinitionID->Get());
        const std::string* pstr_nym_id = &RecordList::s_blank;
        const std::string* pstr_notary_id = &RecordList::s_blank;
        const std::string* pstr_instrument_definition_id = &RecordList::s_blank;
        const std::string* pstr_asset_name = &RecordList::s_blank;
        // NOTE: Since this account is already on my "care about" list for
        // accounts,
        // I wouldn't bother double-checking my "care about" lists for servers,
        // nyms,
        // and instrument definitions. But I still look up the appropriate
        // string for each,
        // since
        // I have to pass a reference to it into the constructor for Record.
        // (To a version
        // that won't be deleted, since the Record will reference it. And the
        // "care about"
        // list definitely contains a copy of the string that won't be deleted.)
        //
        auto it_nym = std::find(m_nyms.begin(), m_nyms.end(), str_nym_id);
        auto it_server =
            std::find(m_servers.begin(), m_servers.end(), str_notary_id);
        auto it_asset = m_assets.find(str_instrument_definition_id);
        if ((m_nyms.end() == it_nym) || (m_servers.end() == it_server) ||
            (m_assets.end() == it_asset)) {
            LogVerbose(OT_METHOD)(__FUNCTION__)(": Skipping an account (")(
                str_account_id.c_str())(") since its Nym, or Server, "
                                        "or Asset Type wasn't on my list.")
                .Flush();
            continue;
        }
        // These pointers are what we'll use to construct each Record.
        //
        pstr_nym_id = &(*it_nym);
        pstr_notary_id = &(*it_server);
        pstr_instrument_definition_id = &(it_asset->first);
        pstr_asset_name = &(it_asset->second);
        // Loop through asset account INBOX.
        //
        // OPTIMIZE FYI:
        // NOTE: LoadInbox is much SLOWER than LoadInboxNoVerify, but it also
        // lets you get
        // the NAME off of the box receipt. So if you are willing to GIVE UP the
        // NAME, in
        // return for FASTER PERFORMANCE, then call SetFastMode() before
        // Populating.
        //
        std::unique_ptr<Ledger> pInbox{nullptr};

        if (false == theNymID.empty()) {
            pInbox = m_bRunFast ? Opentxs::Client().OTAPI().LoadInboxNoVerify(
                                      theNotaryID, theNymID, theAccountID)
                                : Opentxs::Client().OTAPI().LoadInbox(
                                      theNotaryID, theNymID, theAccountID);
        }

        // It loaded up, so let's loop through it.
        if (false != bool(pInbox)) {
            std::int32_t nInboxIndex = -1;
            for (auto& it : pInbox->GetTransactionMap()) {
                ++nInboxIndex;  // (0 on first iteration.)
                if (0 == nInboxIndex)
                    LogVerbose(OT_METHOD)(__FUNCTION__)(
                        ": Beginning loop through asset account INBOX...")
                        .Flush();
                auto pBoxTrans = it.second;
                OT_ASSERT(false != bool(pBoxTrans));
                LogVerbose(OT_METHOD)(__FUNCTION__)(": Inbox index: ")(
                    nInboxIndex)(".")
                    .Flush();
                bool bCanceled = false;
                std::string str_name;  // name of sender (since its in the
                                       // inbox.)
                std::string str_other_nym_id;
                std::string str_other_acct_id;
                std::string str_memo;

                bool bHasSuccess = false;
                bool bIsSuccess = false;

                if (!pBoxTrans->IsAbbreviated()) {
                    auto strMemo = String::Factory();

                    if (pBoxTrans->GetMemo(strMemo)) str_memo = strMemo->Get();

                    if (transactionType::pending == pBoxTrans->GetType()) {
                        // NOTE: REMOVE THE BELOW CODE. (Found a better way,
                        // above this block.)
                        //                      const OTString
                        //                      strBoxTrans(*pBoxTrans);
                        //                      if (strBoxTrans.Exists())
                        //                         str_memo =
                        //            SwigWrap::Pending_GetNote(*pstr_notary_id,
                        //                                      *pstr_nym_id,
                        //                                      str_account_id,
                        //                                   strBoxTrans.Get());
                        auto theSenderID = Identifier::Factory(),
                             theSenderAcctID = Identifier::Factory();

                        if (pBoxTrans->GetSenderAcctIDForDisplay(
                                theSenderAcctID))  // ACCOUNT name.
                        {
                            if (pBoxTrans->GetSenderNymIDForDisplay(
                                    theSenderID)) {
                                const auto strSenderID =
                                    String::Factory(theSenderID);
                                str_other_nym_id = strSenderID->Get();
                            }
                            const auto strSenderAcctID =
                                String::Factory(theSenderAcctID);
                            const std::string str_sender_acct_id(
                                strSenderAcctID->Get());

                            str_other_acct_id = str_sender_acct_id;

                            auto strNameTemp = String::Factory();

                            if (str_name.empty()) {
                                strNameTemp->Format(
                                    RecordList::textFrom(),
                                    str_sender_acct_id.c_str());
                                str_name = strNameTemp->Get();
                            }
                        } else if (pBoxTrans->GetSenderNymIDForDisplay(
                                       theSenderID))  // NYM name.
                        {
                            const auto strSenderID =
                                String::Factory(theSenderID);
                            const std::string str_sender_id(strSenderID->Get());
                            auto strNameTemp = String::Factory();
                            strNameTemp->Format(
                                RecordList::textFrom(), str_sender_id.c_str());
                            str_name = strNameTemp->Get();
                            str_other_nym_id = str_sender_id;
                        } else {
                            auto strName = String::Factory(
                                     SwigWrap::GetAccountWallet_Name(
                                         str_account_id)),
                                 strNameTemp = String::Factory();

                            if (strName->Exists())
                                strNameTemp = strName;
                            else
                                strNameTemp = String::Factory(str_account_id);

                            str_name = strNameTemp->Get();
                        }
                    }     // end: (if pending)
                    else  // else it's a receipt.
                    {
                        pBoxTrans->GetSuccess(&bHasSuccess, &bIsSuccess);
                        // -------------------------------
                        auto theRecipientID = Identifier::Factory(),
                             theRecipientAcctID = Identifier::Factory();

                        if (pBoxTrans->GetRecipientNymIDForDisplay(
                                theRecipientID)) {
                            const auto strRecipientID =
                                String::Factory(theRecipientID);
                            const std::string str_recipient_nym_id(
                                strRecipientID->Get());
                            auto strNameTemp = String::Factory();
                            strNameTemp->Format(
                                RecordList::textTo(),
                                str_recipient_nym_id.c_str());
                            str_name = strNameTemp->Get();
                            str_other_nym_id = str_recipient_nym_id;

                            if (pBoxTrans->GetRecipientAcctIDForDisplay(
                                    theRecipientAcctID)) {
                                const auto strRecipientAcctID =
                                    String::Factory(theRecipientAcctID);
                                str_other_acct_id = strRecipientAcctID->Get();
                            }
                        } else if (pBoxTrans->GetRecipientAcctIDForDisplay(
                                       theRecipientAcctID)) {
                            const auto strRecipientAcctID =
                                String::Factory(theRecipientAcctID);
                            const std::string str_recipient_acct_id(
                                strRecipientAcctID->Get());
                            auto strNameTemp = String::Factory();
                            strNameTemp->Format(
                                RecordList::textTo(),
                                str_recipient_acct_id.c_str());
                            str_name = strNameTemp->Get();
                            str_other_acct_id = str_recipient_acct_id;
                        }
                    }  // end: (else it's a receipt.)
                }
                bCanceled = pBoxTrans->IsCancelled();
                time64_t tValidFrom = OT_TIME_ZERO, tValidTo = OT_TIME_ZERO;
                std::string str_date =
                    "0";  // the "date signed" on the transaction receipt.
                time64_t tDateSigned = pBoxTrans->GetDateSigned();

                if (tDateSigned > OT_TIME_ZERO) {
                    tValidFrom = tDateSigned;
                    const std::uint64_t lDateSigned =
                        OTTimeGetSecondsFromTime(tDateSigned);
                    auto strDateSigned = String::Factory();
                    strDateSigned->Format("%" PRIu64 "", lDateSigned);
                    str_date = strDateSigned->Get();
                }
                std::string str_amount;  // <========== AMOUNT
                Amount lAmount = pBoxTrans->GetAbbrevDisplayAmount();

                if (0 == lAmount) lAmount = pBoxTrans->GetReceiptAmount();
                if (0 != lAmount) {
                    auto strTemp = String::Factory();
                    strTemp->Format("%" PRId64 "", lAmount);
                    str_amount = strTemp->Get();
                }
                const std::string str_type(
                    pBoxTrans->GetTypeString());  // pending, chequeReceipt,
                                                  // etc.
                LogVerbose(OT_METHOD)(__FUNCTION__)(": ADDED: incoming ")(
                    (transactionType::pending == pBoxTrans->GetType())
                        ? "pending transfer"
                        : "receipt")(" (str_type: ")(str_type.c_str())(").")
                    .Flush();

                shared_ptr_Record sp_Record(new Record(
                    *this,
                    *pstr_notary_id,
                    *pstr_notary_id,
                    *pstr_instrument_definition_id,
                    *pstr_asset_name,
                    *pstr_nym_id,    // This is the Nym WHOSE BOX IT IS.
                    str_account_id,  // This is the Nym's account for this
                                     // box.
                    // Everything above this line, it stores a reference to
                    // an external string.
                    // Everything below this line, it makes its own internal
                    // copy of the string.
                    str_name,  // name of sender (since its in the inbox.)
                    str_date,  // the "valid from" date on the instrument.
                    str_amount,
                    str_type,  // pending, chequeReceipt, etc.
                    (transactionType::pending ==
                     pBoxTrans->GetType()),  // Sometimes true, often false.
                    (lAmount < 0) ? true : false,  // bIsOutgoing (this is
                                                   // the inbox, but a
                    // transferReceipt in the inbox represents
                    // outgoing funds. Whereas a "pending" in the
                    // inbox represents incoming funds. For now I'm
                    // just going to go based on whether the amount
                    // is negative or not, to determine incoming /
                    // outgoing. We'll see how that works.)
                    false,  // IsRecord
                    (transactionType::pending !=
                     pBoxTrans->GetType()),  // IsReceipt,
                    (transactionType::pending == pBoxTrans->GetType())
                        ? Record::Transfer
                        : Record::Receipt));

                const auto strContents = String::Factory(*pBoxTrans);
                sp_Record->SetContents(strContents->Get());
                sp_Record->SetDateRange(tValidFrom, tValidTo);
                sp_Record->SetBoxIndex(nInboxIndex);

                if (bCanceled) sp_Record->SetCanceled();
                if (!str_memo.empty()) sp_Record->SetMemo(str_memo);
                if (!str_other_nym_id.empty())
                    sp_Record->SetOtherNymID(str_other_nym_id);
                if (!str_other_acct_id.empty())
                    sp_Record->SetOtherAccountID(str_other_acct_id);

                sp_Record->SetTransNumForDisplay(
                    pBoxTrans->GetReferenceNumForDisplay());
                sp_Record->SetTransactionNum(pBoxTrans->GetTransactionNum());

                if (bHasSuccess) sp_Record->SetSuccess(bIsSuccess);

                //                otErr << "DEBUGGING! Added " <<
                //                pBoxTrans->GetTypeString() <<
                //                "\n pBoxTrans->GetTransactionNum(): " <<
                //                pBoxTrans->GetTransactionNum() <<
                //                "\n pBoxTrans->GetReferenceNumForDisplay()" <<
                //                pBoxTrans->GetReferenceNumForDisplay() <<
                //                "\n";

                m_contents.push_back(sp_Record);
            }
        }
        // OPTIMIZE FYI:
        // NOTE: LoadOutbox is much SLOWER than LoadOutboxNoVerify, but it also
        // lets you get the NAME off of the box receipt. So if you are willing
        // to GIVE UP the NAME, in return for FASTER PERFORMANCE, then call
        // SetFastMode() before running Populate.
        //
        std::unique_ptr<Ledger> pOutbox{nullptr};

        if (false == theNymID.empty()) {
            pOutbox = m_bRunFast ? Opentxs::Client().OTAPI().LoadOutboxNoVerify(
                                       theNotaryID, theNymID, theAccountID)
                                 : Opentxs::Client().OTAPI().LoadOutbox(
                                       theNotaryID, theNymID, theAccountID);
        }

        // It loaded up, so let's loop through it.
        if (false != bool(pOutbox)) {
            std::int32_t nOutboxIndex{-1};
            for (auto& it : pOutbox->GetTransactionMap()) {
                ++nOutboxIndex;  // (0 on first iteration.)
                if (0 == nOutboxIndex)
                    LogVerbose(OT_METHOD)(__FUNCTION__)(
                        ": Beginning loop through asset account OUTBOX...")
                        .Flush();
                auto pBoxTrans = it.second;
                OT_ASSERT(false != bool(pBoxTrans));
                LogVerbose(OT_METHOD)(__FUNCTION__)(": Outbox index: ")(
                    nOutboxIndex)(".")
                    .Flush();
                std::string str_name;  // name of recipient (since its in the
                                       // outbox.)
                std::string str_other_nym_id;
                std::string str_other_acct_id;
                std::string str_memo;

                if (!pBoxTrans->IsAbbreviated()) {
                    auto theRecipientID = Identifier::Factory(),
                         theRecipientAcctID = Identifier::Factory();

                    if (pBoxTrans->GetRecipientNymIDForDisplay(
                            theRecipientID)) {
                        const auto strRecipientID =
                            String::Factory(theRecipientID);
                        const std::string str_recipient_id(
                            strRecipientID->Get());
                        auto strNameTemp = String::Factory();
                        strNameTemp->Format(
                            RecordList::textTo(), str_recipient_id.c_str());
                        str_name = strNameTemp->Get();
                        str_other_nym_id = str_recipient_id;
                        if (pBoxTrans->GetRecipientAcctIDForDisplay(
                                theRecipientAcctID)) {
                            const auto strRecipientAcctID =
                                String::Factory(theRecipientAcctID);
                            str_other_acct_id = strRecipientAcctID->Get();
                        }
                    } else if (pBoxTrans->GetRecipientAcctIDForDisplay(
                                   theRecipientAcctID)) {
                        const auto strRecipientAcctID =
                            String::Factory(theRecipientAcctID);
                        const std::string str_recipient_acct_id(
                            strRecipientAcctID->Get());
                        auto strNameTemp = String::Factory();
                        strNameTemp->Format(
                            RecordList::textTo(),
                            str_recipient_acct_id.c_str());
                        str_name = strNameTemp->Get();
                        str_other_acct_id = str_recipient_acct_id;
                    }
                    if (transactionType::pending == pBoxTrans->GetType()) {
                        auto strMemo = String::Factory();

                        if (pBoxTrans->GetMemo(strMemo))
                            str_memo = strMemo->Get();

                        // DELETE THE BELOW CODE (replaced by above code.)
                        //                    const OTString
                        //                    strBoxTrans(*pBoxTrans);
                        //
                        //                    if (strBoxTrans.Exists())
                        //                        str_memo =
                        //                        SwigWrap::Pending_GetNote(*pstr_notary_id,
                        //                            *pstr_nym_id,
                        //                            str_account_id,
                        //                            strBoxTrans.Get());
                    }
                }
                time64_t tValidFrom = OT_TIME_ZERO, tValidTo = OT_TIME_ZERO;
                std::string str_date =
                    "0";  // the "date signed" on the transaction receipt.
                time64_t tDateSigned = pBoxTrans->GetDateSigned();

                if (tDateSigned > OT_TIME_ZERO) {
                    tValidFrom = tDateSigned;
                    const std::uint64_t lDateSigned =
                        OTTimeGetSecondsFromTime(tDateSigned);
                    auto strDateSigned = String::Factory();
                    strDateSigned->Format("%" PRIu64 "", lDateSigned);
                    str_date = strDateSigned->Get();
                }
                std::string str_amount;  // <========== AMOUNT
                Amount lAmount = pBoxTrans->GetAbbrevDisplayAmount();

                if (0 == lAmount) lAmount = pBoxTrans->GetReceiptAmount();
                if (lAmount > 0)  // Outgoing transfer should display with
                                  // negative amount
                    lAmount *= (-1);
                if (0 != lAmount) {
                    auto strTemp = String::Factory();
                    strTemp->Format("%" PRId64 "", lAmount);
                    str_amount = strTemp->Get();
                }
                std::string str_type(pBoxTrans->GetTypeString());  // pending,
                                                                   // chequeReceipt,
                                                                   // etc.
                if (0 == str_type.compare("pending")) str_type = "transfer";
                LogVerbose(OT_METHOD)(__FUNCTION__)(": ADDED: ")(
                    (transactionType::pending == pBoxTrans->GetType())
                        ? "pending"
                        : "ERROR")(" outgoing transfer (str_type: ")(
                    str_type.c_str())(").")
                    .Flush();

                shared_ptr_Record sp_Record(new Record(
                    *this,
                    *pstr_notary_id,  // Transport Notary.
                    *pstr_notary_id,  // Payment Notary.
                    *pstr_instrument_definition_id,
                    *pstr_asset_name,
                    *pstr_nym_id,    // This is the Nym WHOSE BOX IT IS.
                    str_account_id,  // This is the Nym's account for this box.
                    // Everything above this line, it stores a reference to
                    // an external string.
                    // Everything below this line, it makes its own internal
                    // copy of the string.
                    str_name,  // name of recipient (since its in the
                               // outbox.)
                    str_date,  // the "valid from" date on the instrument.
                    str_amount,
                    str_type,  // pending, chequeReceipt, etc.
                    (transactionType::pending ==
                     pBoxTrans->GetType()),  // Basically always true, in
                                             // this case.
                    true,   // bIsOutgoing=true. (Since this is the
                            // outbox...)
                    false,  // IsRecord
                    false,  // IsReceipt
                    Record::Transfer));
                const auto strContents = String::Factory(*pBoxTrans);
                sp_Record->SetContents(strContents->Get());
                sp_Record->SetDateRange(tValidFrom, tValidTo);
                sp_Record->SetBoxIndex(nOutboxIndex);
                if (!str_memo.empty()) sp_Record->SetMemo(str_memo);
                if (!str_other_nym_id.empty())
                    sp_Record->SetOtherNymID(str_other_nym_id);
                if (!str_other_acct_id.empty())
                    sp_Record->SetOtherAccountID(str_other_acct_id);
                sp_Record->SetTransNumForDisplay(
                    pBoxTrans->GetReferenceNumForDisplay());
                sp_Record->SetTransactionNum(pBoxTrans->GetTransactionNum());

                //                otErr << "DEBUGGING! Added outgoing asset
                //                account record: " <<
                //                pBoxTrans->GetTypeString() <<
                //                "\n pBoxTrans->GetTransactionNum(): " <<
                //                pBoxTrans->GetTransactionNum() <<
                //                "\n pBoxTrans->GetReferenceNumForDisplay()" <<
                //                pBoxTrans->GetReferenceNumForDisplay() <<
                //                "\n";

                m_contents.push_back(sp_Record);
            }
        }
        // ---------------------------------------------------
        // For this record box, pass a NymID AND an AcctID,
        // since it's the recordbox for a SPECIFIC ACCOUNT.
        //
        // OPTIMIZE FYI:
        // NOTE: LoadRecordBox is much SLOWER than LoadRecordBoxNoVerify, but it
        // also lets you get the NAME off of the box receipt. So if you are
        // willing to GIVE UP the NAME, in return for FASTER PERFORMANCE, then
        // call SetFastMode() before Populating.
        //
        std::unique_ptr<Ledger> pRecordbox{nullptr};

        if (false == theNymID.empty()) {
            pRecordbox = m_bRunFast
                             ? Opentxs::Client().OTAPI().LoadRecordBoxNoVerify(
                                   theNotaryID, theNymID, theAccountID)
                             : Opentxs::Client().OTAPI().LoadRecordBox(
                                   theNotaryID, theNymID, theAccountID);
        }

        // It loaded up, so let's loop through it.
        if (false != bool(pRecordbox)) {
            std::int32_t nRecordIndex{-1};
            for (auto& it : pRecordbox->GetTransactionMap()) {
                ++nRecordIndex;
                auto pBoxTrans = it.second;
                OT_ASSERT(false != bool(pBoxTrans));
                LogVerbose(OT_METHOD)(__FUNCTION__)(": Account RECORD index: ")(
                    nRecordIndex)(".")
                    .Flush();
                bool bOutgoing{false};
                bool bCanceled{false};
                std::string str_name;  // name of sender OR recipient (depending
                                       // on whether it was originally incoming
                                       // or outgoing.)
                std::string str_other_nym_id;
                std::string str_other_acct_id;
                std::string str_memo;

                bool bHasSuccess{false};
                bool bIsSuccess{false};

                TransactionNumber lClosingNum{0};
                originType theOriginType = pBoxTrans->GetOriginType();

                const bool bIsFinalReceipt =
                    (transactionType::finalReceipt == pBoxTrans->GetType());
                if (bIsFinalReceipt) lClosingNum = pBoxTrans->GetClosingNum();

                if (!pBoxTrans->IsAbbreviated()) {
                    if (pBoxTrans->GetType() != transactionType::pending)
                        pBoxTrans->GetSuccess(&bHasSuccess, &bIsSuccess);
                    // ----------------------------------------
                    auto theSenderID = Identifier::Factory(),
                         theSenderAcctID = Identifier::Factory();
                    auto theRecipientID = Identifier::Factory(),
                         theRecipientAcctID = Identifier::Factory();

                    if (pBoxTrans->GetSenderAcctIDForDisplay(theSenderAcctID)) {
                        const auto strSenderAcctID =
                            String::Factory(theSenderAcctID);
                        const std::string str_sender_acct_id(
                            strSenderAcctID->Get());

                        // Usually, Nym is the RECIPIENT. Sometimes he's the
                        // sender.
                        // Either way, we want the OTHER ID (the other Nym) for
                        // display.
                        // So here, if Nym's CLEARLY the sender, then we want
                        // the RECIPIENT.
                        // Whereas if Nym were the recipient, then we'd want the
                        // SENDER. (For display.)
                        //
                        if (0 ==
                            str_account_id.compare(
                                str_sender_acct_id))  // str_account_id IS
                                                      // str_sender_acct_id.
                        // (Therefore we want recipient.)
                        {
                            bOutgoing = true;  // if Nym is the sender, then it
                                               // must have been outgoing.

                            const bool bGotRecipientNymIDForDisplay =
                                pBoxTrans->GetRecipientNymIDForDisplay(
                                    theRecipientID);

                            if (pBoxTrans->GetRecipientAcctIDForDisplay(
                                    theRecipientAcctID)) {
                                const auto strRecipientAcctID =
                                    String::Factory(theRecipientAcctID);
                                const std::string str_recip_acct_id(
                                    strRecipientAcctID->Get());

                                auto strRecipientNymID = String::Factory();
                                std::string str_recip_nym_id("");

                                if (bGotRecipientNymIDForDisplay) {
                                    theRecipientID->GetString(
                                        strRecipientNymID);
                                    str_recip_nym_id = strRecipientNymID->Get();
                                }
                                // NOTE: We check for cancelled here so we don't
                                // accidentally
                                // cause the address book to falsely believe
                                // that str_recip_nym_id
                                // is the owner of str_recip_acct_id. (If the
                                // cheque/invoice is cancelled,
                                // the recipient account will be the sender
                                // account, which is NOT owned
                                // by the recipient, obviously...)
                                //
                                if (!pBoxTrans->IsCancelled()) {
                                    auto strNameTemp = String::Factory();
                                    strNameTemp->Format(
                                        RecordList::textTo(),
                                        str_recip_acct_id.c_str());
                                    str_name =
                                        strNameTemp->Get();  // We don't want to
                                                             // see our own name
                                                             // on cancelled
                                                             // cheques.
                                }
                                str_other_acct_id = str_recip_acct_id;
                            }
                            if (bGotRecipientNymIDForDisplay) {
                                const auto strRecipientID =
                                    String::Factory(theRecipientID);
                                const std::string str_recipient_id(
                                    strRecipientID->Get());

                                str_other_nym_id = str_recipient_id;

                                if (str_name.empty()) {
                                    auto strNameTemp = String::Factory();
                                    strNameTemp->Format(
                                        RecordList::textTo(),
                                        str_recipient_id.c_str());
                                    str_name = strNameTemp->Get();
                                }
                            }
                        } else  // str_account_id IS NOT str_sender_acct_id.
                                // (Therefore we want sender.)
                        {  // In this case, some OTHER Nym is the sender, so it
                            // must have been incoming. (And bOutgoing is
                            // already false.)

                            if (pBoxTrans->GetSenderNymIDForDisplay(
                                    theSenderID)) {
                                const auto strSenderNymID =
                                    String::Factory(theSenderID);
                                str_other_nym_id = strSenderNymID->Get();
                            }
                            auto strNameTemp = String::Factory();
                            strNameTemp->Format(
                                RecordList::textFrom(),
                                str_sender_acct_id.c_str());
                            str_name = strNameTemp->Get();
                            str_other_acct_id = str_sender_acct_id;
                        }
                    }
                    // In this block below, we already KNOW
                    // GetSenderAcctIDForDisplay is EMPTY.
                    // (So it's "recipient or bust.")
                    else if (pBoxTrans->GetRecipientAcctIDForDisplay(
                                 theRecipientAcctID)) {
                        if (pBoxTrans->GetRecipientNymIDForDisplay(
                                theRecipientID)) {
                            const auto strRecipientID =
                                String::Factory(theRecipientID);
                            const std::string str_recipient_nym_id(
                                strRecipientID->Get());

                            str_other_nym_id = str_recipient_nym_id;
                        }
                        const auto strRecipientAcctID =
                            String::Factory(theRecipientAcctID);
                        const std::string str_recipient_acct_id(
                            strRecipientAcctID->Get());

                        if (0 != str_account_id.compare(
                                     str_recipient_acct_id))  // str_account_id
                                                              // is NOT
                        // str_recipient_acct_id.
                        {  // (Therefore we want str_recipient_acct_id.)
                            // If Nym is not the recipient, then he must be the
                            // sender.
                            // (Therefore it must be outgoing.)
                            bOutgoing = true;

                            auto strNameTemp = String::Factory();
                            strNameTemp->Format(
                                RecordList::textTo(),
                                str_recipient_acct_id.c_str());
                            str_name = strNameTemp->Get();
                            str_other_acct_id = str_recipient_acct_id;
                        }
                    } else if (pBoxTrans->GetSenderNymIDForDisplay(
                                   theSenderID)) {
                        const auto strSenderID = String::Factory(theSenderID);
                        const std::string str_sender_id(strSenderID->Get());

                        // Usually, Nym is the RECIPIENT. Sometimes he's the
                        // sender.
                        // Either way, we want the OTHER ID (the other Nym) for
                        // display.
                        // So here, if Nym's CLEARLY the sender, then we want
                        // the RECIPIENT.
                        // Whereas if Nym were the recipient, then we'd want the
                        // SENDER. (For display.)
                        //
                        if (0 == str_nym_id.compare(
                                     str_sender_id))  // str_nym_id IS
                                                      // str_sender_id.
                        // (Therefore we want recipient.)
                        {
                            bOutgoing = true;  // if Nym is the sender, then it
                                               // must have been outgoing.

                            if (pBoxTrans->GetRecipientNymIDForDisplay(
                                    theRecipientID)) {
                                const auto strRecipientID =
                                    String::Factory(theRecipientID);
                                const std::string str_recipient_id(
                                    strRecipientID->Get());
                                auto strNameTemp = String::Factory();
                                strNameTemp->Format(
                                    RecordList::textTo(),
                                    str_recipient_id.c_str());
                                str_name = strNameTemp->Get();
                                str_other_nym_id = str_recipient_id;

                                if (pBoxTrans->GetRecipientAcctIDForDisplay(
                                        theRecipientAcctID)) {
                                    const auto strRecipientAcctID =
                                        String::Factory(theRecipientAcctID);
                                    str_other_acct_id =
                                        strRecipientAcctID->Get();
                                }
                            }
                        } else  // str_nym_id IS NOT str_sender_id. (Therefore
                                // we want sender.)
                        {  // In this case, some OTHER Nym is the sender, so it
                            // must have been incoming. (And bOutgoing is
                            // already false.)
                            auto strNameTemp = String::Factory();
                            strNameTemp->Format(
                                RecordList::textFrom(), str_sender_id.c_str());
                            str_name = strNameTemp->Get();
                            str_other_nym_id = str_sender_id;

                            if (pBoxTrans->GetSenderAcctIDForDisplay(
                                    theSenderAcctID)) {
                                const auto strSenderAcctID =
                                    String::Factory(theSenderAcctID);
                                str_other_acct_id = strSenderAcctID->Get();
                            }
                        }
                    }
                    // In this block below, we already KNOW
                    // GetSenderNymIDForDisplay is EMPTY.
                    // (So it's "recipient or bust.")
                    else if (pBoxTrans->GetRecipientNymIDForDisplay(
                                 theRecipientID)) {
                        const auto strRecipientID =
                            String::Factory(theRecipientID);
                        const std::string str_recipient_id(
                            strRecipientID->Get());

                        if (0 !=
                            str_nym_id.compare(str_recipient_id))  // str_nym_id
                                                                   // is NOT
                        // str_recipient_id.
                        {  // (Therefore we want str_recipient_id.)
                            // If Nym is not the recipient, then he must be the
                            // sender.
                            // (Therefore it must be outgoing.)
                            bOutgoing = true;
                            auto strNameTemp = String::Factory();
                            strNameTemp->Format(
                                RecordList::textTo(), str_recipient_id.c_str());
                            str_name = strNameTemp->Get();
                            str_other_nym_id = str_recipient_id;

                            if (pBoxTrans->GetRecipientAcctIDForDisplay(
                                    theRecipientAcctID)) {
                                const auto strRecipientAcctID =
                                    String::Factory(theRecipientAcctID);
                                str_other_acct_id = strRecipientAcctID->Get();
                            }
                        }
                    }
                    // Get the Memo field for a transferReceipt and also for
                    // other receipts.
                    //
                    auto strMemo = String::Factory();

                    if (pBoxTrans->GetMemo(strMemo)) str_memo = strMemo->Get();

                }  // if not abbreviated.
                bCanceled = pBoxTrans->IsCancelled();
                time64_t tValidFrom = OT_TIME_ZERO, tValidTo = OT_TIME_ZERO;
                std::string str_date =
                    "0";  // the "date signed" on the transaction receipt.
                time64_t tDateSigned = pBoxTrans->GetDateSigned();

                if (tDateSigned > OT_TIME_ZERO) {
                    tValidFrom = tDateSigned;
                    const std::uint64_t lDateSigned =
                        OTTimeGetSecondsFromTime(tDateSigned);
                    auto strDateSigned = String::Factory();
                    strDateSigned->Format("%" PRIu64 "", lDateSigned);
                    str_date = strDateSigned->Get();
                }
                std::string str_amount;  // <========== AMOUNT
                Amount lAmount = pBoxTrans->GetAbbrevDisplayAmount();

                if (0 == lAmount) lAmount = pBoxTrans->GetReceiptAmount();
                const std::string str_type(
                    pBoxTrans->GetTypeString());  // pending, chequeReceipt,
                                                  // etc.
                if (0 == str_type.compare("transferReceipt"))
                    bOutgoing = true;  // only the sender of a transfer will
                                       // have a transferReceipt. (The recipient
                                       // will have the recipient will have his
                                       // process inbox receipt from when he
                                       // accepts the incoming transfer).
                else if (0 == str_type.compare("pending"))
                    bOutgoing = false;  // only the recipient of a transfer will
                                        // have a pending in his recordbox.
                if (0 != lAmount) {
                    //              if (lAmount < 0)
                    //                  bOutgoing = true;
                    //              else
                    //                  bOutgoing = false;

                    // A transfer receipt ALWAYS represents an outgoing
                    // transfer.
                    // If the amount is over 0, we want to display it as a
                    // negative since it represents money LEAVING my account.
                    // if ((0 == str_type.compare("transferReceipt")) &&
                    //     (lAmount > 0))
                    //     lAmount *= (-1);

                    auto strTemp = String::Factory();
                    strTemp->Format("%" PRId64 "", lAmount);
                    str_amount = strTemp->Get();
                }
                LogVerbose(OT_METHOD)(__FUNCTION__)(": ADDED: ")(
                    (pBoxTrans->GetType() != transactionType::pending)
                        ? ""
                        : (bOutgoing ? "sent" : "received"))(
                    " (asset account) record (str_type: ")(str_type.c_str())(
                    ").")
                    .Flush();
                // This line means: If it's a receipt, use a blank string.
                // Otherwise if it's a transfer, then show sent/received. (This
                // is the record box, so if it's a transfer, it's a completed
                // one.)
                //
                // FYI, for Receipts we don't say "sent transferReceipt",
                // we just say "transferReceipt."

                shared_ptr_Record sp_Record(new Record(
                    *this,
                    *pstr_notary_id,
                    *pstr_notary_id,
                    *pstr_instrument_definition_id,
                    *pstr_asset_name,
                    *pstr_nym_id,    // This is the Nym WHOSE BOX IT IS.
                    str_account_id,  // This is the Nym's account for this
                                     // box.
                    // Everything above this line, it stores a reference to
                    // an
                    // external string.
                    // Everything below this line, it makes its own internal
                    // copy of the string.
                    str_name,  // name of sender or recipient (whichever is
                               // NOT
                               // the current Nym.)
                    str_date,  // the "valid from" date on the instrument.
                    str_amount,
                    str_type,   // pending, chequeReceipt, etc.
                    false,      // bPending=false. If it's in the record box,
                                // then
                                // it's finished (not pending.)
                    bOutgoing,  // Record box stores both old incoming, AND
                                // old
                                // outgoing, receipts.
                    true,       // IsRecord
                    pBoxTrans->GetType() !=
                        transactionType::pending,  // IsReceipt
                    pBoxTrans->GetType() == transactionType::pending
                        ? Record::Transfer
                        : Record::Receipt));
                const auto strContents = String::Factory(*pBoxTrans);
                sp_Record->SetContents(strContents->Get());
                if (bCanceled) sp_Record->SetCanceled();
                sp_Record->SetDateRange(tValidFrom, tValidTo);
                sp_Record->SetBoxIndex(nRecordIndex);
                if (!str_memo.empty()) sp_Record->SetMemo(str_memo);
                if (!str_other_nym_id.empty())
                    sp_Record->SetOtherNymID(str_other_nym_id);
                if (!str_other_acct_id.empty())
                    sp_Record->SetOtherAccountID(str_other_acct_id);
                sp_Record->SetTransNumForDisplay(
                    pBoxTrans->GetReferenceNumForDisplay());
                sp_Record->SetTransactionNum(pBoxTrans->GetTransactionNum());

                if (bHasSuccess) sp_Record->SetSuccess(bIsSuccess);

                if (bIsFinalReceipt) {
                    sp_Record->SetFinalReceipt();
                    sp_Record->SetClosingNum(lClosingNum);
                }
                sp_Record->SetOriginType(theOriginType);

                //              otErr << "DEBUGGING! Added " << (bOutgoing ?
                //              "sent": "received") << " asset account record:
                //              " <<
                //              pBoxTrans->GetTypeString() <<
                //              "\n pBoxTrans->GetTransactionNum(): " <<
                //              pBoxTrans->GetTransactionNum() <<
                //              "\n pBoxTrans->GetReferenceNumForDisplay()" <<
                //              pBoxTrans->GetReferenceNumForDisplay() <<
                //              "\n";

                m_contents.push_back(sp_Record);
            }
        }

    }  // loop through the accounts.
    // SORT the vector.
    //
    SortRecords();
    return true;
}

const list_of_strings& RecordList::GetNyms() const { return m_nyms; }

// Populate already sorts. But if you have to add some external records
// after Populate, then you can sort again.
//
void RecordList::SortRecords()
{
    // TODO OPTIMIZE: We might load everything up into a multimap, and THEN copy
    // it
    // directly over to the vector. (Since the multimap sorts automatically on
    // insert.)
    // The question is, would doing that be any faster than just sorting it
    // here?
    // (Possibly not, but I'm not sure. Re-visit later.)
    //
    // Todo optimize: any faster sorting algorithms?
    std::sort(
        m_contents.begin(),
        m_contents.end(),
        [](const shared_ptr_Record& i, const shared_ptr_Record& j) {
            return j->operator<(*i);
        });
}

// Let's say you also want to add some Bitmessages. (Or any other external
// source.) This is where you do that. Make sure to call Populate, then use
// AddSpecialMsg a few times, then call SortRecords.
//
void RecordList::AddSpecialMsg(
    const std::string& str_msg_id,  // The id of this message, from whatever
                                    // system it came from.
    bool bIsOutgoing,
    std::int32_t nMethodID,
    const std::string& str_contents,  // Make sure to concatentate subject with
                                      // contents, before passing here.
    const std::string& str_address,
    const std::string& str_other_address,
    const std::string& str_type,
    const std::string& str_type_display,
    std::string str_my_nym_id,
    time64_t tDate)
{
    const char* pToFrom =
        bIsOutgoing ? RecordList::textTo() : RecordList::textFrom();
    const std::string* p_str_server =
        &RecordList::s_blank;  // <========== Bitmessage doesn't use OT
                               // servers.
    // TODO OPTIMIZE: instead of looking up the Nym's name every time, look it
    // up ONCE when first adding the NymID. Add it to a map, instead of a list,
    // and add the Nym's name as the second item in the map's pair.
    // (Just like I already did with the instrument definition.)
    //
    std::string str_other_name;
    auto strNameTemp = String::Factory();
    std::string str_name("");

    if (!str_other_name.empty())
        strNameTemp->Format(pToFrom, str_other_name.c_str());
    else if (!str_other_address.empty())
        strNameTemp->Format(pToFrom, str_other_address.c_str());

    str_name = strNameTemp->Get();
    const std::string* p_str_nym_id =
        &RecordList::s_blank;  // <========== MY NYM ID
    const std::string* p_str_asset_type =
        &RecordList::s_blank;  // <========== ASSET TYPE
    const std::string* p_str_asset_name =
        &RecordList::s_blank;  // instrument definition display name.
    const std::string* p_str_account =
        &RecordList::s_blank;  // <========== ACCOUNT

    std::string str_amount;  // There IS NO amount, on mail. (So we leave this
                             // empty.)
    // ---------------------------------------------------
    std::uint64_t lDate = static_cast<uint64_t>(tDate);
    auto strDate = String::Factory();
    strDate->Format("%" PRIu64 "", lDate);
    const std::string str_date(strDate->Get());
    // CREATE AN Record AND POPULATE IT...
    //
    // This loop is here because normally an Record's "nym id" is
    // a reference to a string found in RecordList's list of Nyms.
    // We can't just stick a Nym ID in there, we have to find the
    // specific Nym ID string in the RecordList and use THAT, otherwise
    // we'd end up with bad pointer problems.
    //
    if (!str_my_nym_id.empty()) {
        for (auto& it_nym : m_nyms) {
            const std::string& str_nym_id(it_nym);

            if (0 == str_my_nym_id.compare(str_nym_id)) {
                p_str_nym_id = &str_nym_id;
                break;
            }
        }
    }
    shared_ptr_Record sp_Record(new Record(
        *this,
        *p_str_server,
        *p_str_server,
        *p_str_asset_type,
        *p_str_asset_name,
        *p_str_nym_id,   // This is "me" (the sender Nym, if outgoing, or
                         // recipient, if incoming.)
        *p_str_account,  // No OT asset account on any mail, much less on a
                         // "bitmessage" mail.
        // Everything above this line, it stores a reference to an external
        // string.
        // Everything below this line, it makes its own internal copy of the
        // string.
        str_name,  // name of sender for incoming, otherwise name of
                   // recipient for outgoing.
        str_date,
        str_amount,
        RecordList::s_message_type,  // "message"
        false,  // bIsPending=false since its already received.
        bIsOutgoing,
        false,  // IsRecord
        false,  // IsReceipt
        Record::Mail));
    sp_Record->SetSpecialMail();  // true by default. This means it's not an OT
                                  // message, but a Bitmessage (or something
                                  // like that.)
    sp_Record->SetContents(str_contents.c_str());  // "Subject: %s\n[Contents]"
    sp_Record->SetMsgID(str_msg_id);
    sp_Record->SetDateRange(
        OTTimeGetTimeFromSeconds(tDate), OTTimeGetTimeFromSeconds(tDate));
    sp_Record->SetMethodID(nMethodID);
    sp_Record->SetAddress(str_address);
    sp_Record->SetOtherAddress(str_other_address);
    sp_Record->SetMsgType(str_type);
    sp_Record->SetMsgTypeDisplay(str_type_display);
    LogDetail(OT_METHOD)(__FUNCTION__)(": ADDED: ")(
        bIsOutgoing ? "outgoing" : "incoming")(" special mail.")
        .Flush();

    m_contents.push_back(sp_Record);
}

// Clears m_contents (NOT nyms, accounts, servers, or instrument definitions.)

void RecordList::ClearContents() { m_contents.clear(); }

// RETRIEVE:
//

std::int32_t RecordList::size() const { return m_contents.size(); }

bool RecordList::RemoveRecord(std::int32_t nIndex)
{
    OT_ASSERT(
        (nIndex >= 0) &&
        (nIndex < static_cast<std::int32_t>(m_contents.size())));
    m_contents.erase(m_contents.begin() + nIndex);
    return true;
}

Record RecordList::GetRecord(std::int32_t nIndex)
{
    OT_ASSERT(
        (nIndex >= 0) &&
        (nIndex < static_cast<std::int32_t>(m_contents.size())));
    return *(m_contents[nIndex]);
}

}  // namespace opentxs::cli
