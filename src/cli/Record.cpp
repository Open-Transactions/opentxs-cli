// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "Record.hpp"

#include "RecordList.hpp"

#include <algorithm>
#include <cinttypes>
#include <cstdint>
#include <memory>
#include <ostream>
#include <string>

namespace opentxs::cli
{

bool Record::FormatAmount(std::string& str_output) const
{
    if (m_str_amount.empty() || m_str_unit_type_id.empty())  // Need these to do
                                                             // the formatting.
    {
        //      otOut << __FUNCTION__ << ": Unable to format amount. Type: " <<
        //      m_str_type << " Amount: "
        //            << m_str_amount << "  Asset: " <<
        //            m_str_unit_type_id << "";
        return false;
    }
    str_output = Opentxs::Client().Exec().FormatAmount(
        m_str_unit_type_id,
        Opentxs::Client().Exec().StringToLong(m_str_amount));
    return (!str_output.empty());
}

bool Record::FormatAmountWithoutSymbol(std::string& str_output)
{
    if (m_str_amount.empty() || m_str_unit_type_id.empty()) { return false; }

    str_output = Opentxs::Client().Exec().FormatAmountWithoutSymbol(
        m_str_unit_type_id,
        Opentxs::Client().Exec().StringToLong(m_str_amount));
    return (!str_output.empty());
}

bool Record::FormatAmountLocale(
    std::string& str_output,
    const std::string& str_thousands,
    const std::string& str_decimal) const
{
    if (m_str_amount.empty() || m_str_unit_type_id.empty())  // Need these to do
                                                             // the formatting.
    {
        //      otOut << __FUNCTION__ << ": Unable to format amount. Type: " <<
        //      m_str_type << " Amount: "
        //            << m_str_amount << "  Asset: " <<
        //            m_str_unit_type_id << "";
        return false;
    }
    str_output = Opentxs::Client().Exec().FormatAmountLocale(
        m_str_unit_type_id,
        Opentxs::Client().Exec().StringToLong(m_str_amount),
        str_thousands,
        str_decimal);
    return (!str_output.empty());
}

bool Record::FormatAmountWithoutSymbolLocale(
    std::string& str_output,
    const std::string& str_thousands,
    const std::string& str_decimal)
{
    if (m_str_amount.empty() || m_str_unit_type_id.empty()) { return false; }

    str_output = Opentxs::Client().Exec().FormatAmountWithoutSymbolLocale(
        m_str_unit_type_id,
        Opentxs::Client().Exec().StringToLong(m_str_amount),
        str_thousands,
        str_decimal);
    return (!str_output.empty());
}

bool Record::FormatMailSubject(std::string& str_output) const
{
    str_output.clear();
    if (IsMail()) {
        if (!HasContents())
            str_output = "(empty subject)";
        else {
            std::string str_temp_contents = GetContents();

            if (str_temp_contents.compare(0, 8, "Subject:") == 0) {
                // Make the replacement.
                str_temp_contents.replace(0, 8, "");
            }
            // Trim it, since there is probably a space after "Subject:"
            // (Plus we want it trimmed anyway.)
            std::string str_contents = String::trim(str_temp_contents);
            // Cut the string at the first newline.
            //
            std::string::size_type pos_start = 0;
            std::string::size_type pos = str_contents.find("\r\n", pos_start);

            if (std::string::npos == pos)  // Didn't find anything.
                pos = str_contents.find("\n", pos_start);
            if (std::string::npos != pos)  // We found a newline.
                str_contents.erase(pos, std::string::npos);
            // Trim it again, just for good measure.
            str_output = String::trim(str_contents);
        }
    }
    return (!str_output.empty());
}

bool Record::FormatShortMailDescription(std::string& str_output) const
{
    String strDescription;

    if (IsMail()) {
        if (!HasContents())
            strDescription.Set("(empty message)");
        else {
            std::string str_temp_contents = GetContents();
            std::string str_contents = String::trim(str_temp_contents);

            if (str_contents.compare(0, 8, "Subject:") == 0) {
                // Make the replacement.
                str_contents.replace(0, 8, "");
            }
            bool bTruncated = false;

            if (str_contents.size() > 30) {
                str_contents.erase(30, std::string::npos);
                bTruncated = true;
            }
            // Replace any newlines with spaces...
            //
            std::replace(str_contents.begin(), str_contents.end(), '\r', ' ');
            std::replace(str_contents.begin(), str_contents.end(), '\n', ' ');

            //          str_contents.erase(std::remove(str_contents.begin(),
            // str_contents.end(), '\n'), str_contents.end());
            //          str_contents.erase(std::remove(str_contents.begin(),
            // str_contents.end(), '\r'), str_contents.end());
            strDescription.Format(
                "%s%s",
                String::trim(str_contents).c_str(),
                bTruncated ? "..." : "");
        }
    }
    str_output = strDescription.Get();
    return (!str_output.empty());
}

bool Record::FormatDescription(std::string& str_output) const
{
    bool bIsSuccess = false;
    const bool bHasSuccess = this->HasSuccess(bIsSuccess);
    // ----------------------------------------
    String strDescription, strStatus, strKind;

    if (IsRecord()) {
        if (IsExpired())
            strStatus = "(EXPIRED)";  // How to tell difference between
                                      // instrument that processed successfully
                                      // and THEN expired, versus one that
                                      // expired before processing successfully?
                                      // (See next comment.)
        else if (IsInvoice())
            strStatus =
                "(payment sent)";  // TODO: need an "invalid records" box
                                   // for expired and canceled. Otherwise
                                   // we may falsely assume "payment
                                   // sent" when actually it expired. (We
                                   // may also falsely assume payment
                                   // expired when actually it was sent.)
                                   // Solution is a new box.
        else if (IsNotice()) {
            if (bHasSuccess) {
                if (bIsSuccess)
                    strStatus = "(activated)";
                else
                    strStatus = "(failed activating)";
            }
        } else
            strStatus = "";
        //          strStatus = "(record)";
    } else if (IsPending()) {
        if (IsExpired())
            strStatus = "(EXPIRED)";
        else if (IsInvoice())
            strStatus = "(unpaid)";
        else if (!IsCash())
            strStatus = "(pending)";
    }
    // ----------------------------------------
    if (IsCanceled()) {
        strStatus = "(CANCELED)";

        if (IsOutgoing() || IsReceipt()) strKind.Format("%s", "sent ");
    } else {
        if (IsOutgoing())
            strKind.Format(
                "%s", ((IsPending() && !IsCash()) ? "outgoing " : "sent "));
        else  // Incoming.
            strKind.Format(
                "%s",
                IsPending() ? "incoming " : (IsReceipt() ? "" : "received "));
    }
    // ----------------------------------------
    String strTransNumForDisplay;

    if (!IsCash())
        strTransNumForDisplay.Format(" #%" PRId64, GetTransNumForDisplay());
    // ----------------------------------------
    if (IsRecord()) {
        if (IsTransfer())
            strDescription.Format(
                "%s%s%s %s",
                strKind.Get(),
                "transfer",
                strTransNumForDisplay.Get(),
                strStatus.Get());
        else if (IsVoucher())
            strDescription.Format(
                "%s%s%s %s",
                strKind.Get(),
                "payment",
                strTransNumForDisplay.Get(),
                strStatus.Get());
        else if (IsReceipt()) {
            std::string str_instrument_type;

            if (0 == GetInstrumentType().compare("transferReceipt"))
                str_instrument_type = "transfer";
            else if (0 == GetInstrumentType().compare("finalReceipt")) {
                if (HasOriginType()) {
                    if (IsOriginTypeMarketOffer()) {
                        str_instrument_type = "market offer (finished)";
                    } else if (IsOriginTypePaymentPlan()) {
                        str_instrument_type = "payment plan (finished)";
                    } else if (IsOriginTypeSmartContract()) {
                        str_instrument_type = "smart contract (finished)";
                    } else {
                        str_instrument_type = "final receipt (finished)";
                    }
                } else {
                    str_instrument_type = "final receipt (finished)";
                }
            } else if (0 == GetInstrumentType().compare("marketReceipt")) {
                const Amount lAmount =
                    Opentxs::Client().Exec().StringToLong(m_str_amount);

                // I *think* successful trades have a negative amount -- we'll
                // find out!
                //
                if (lAmount == 0)
                    str_instrument_type = "failed trade";
                else {
                    str_instrument_type = "market trade (receipt)";
                }
            } else if (0 == GetInstrumentType().compare("chequeReceipt")) {
                const Amount lAmount =
                    Opentxs::Client().Exec().StringToLong(m_str_amount);

                // I paid OUT when this chequeReceipt came through. It must be a
                // normal cheque that I wrote.
                if (lAmount <= 0)  // Notice less than OR EQUAL TO 0 -- that's
                                   // because a canceled cheque has a 0 value.
                    str_instrument_type = "cheque";
                else  // I GOT PAID when this chequeReceipt came through. It
                      // must be an invoice I wrote, that someone paid.
                {
                    if (IsCanceled())
                        str_instrument_type = "invoice";
                    else
                        str_instrument_type = "invoice (payment received)";
                }
            } else if (0 == GetInstrumentType().compare("voucherReceipt")) {
                str_instrument_type = "payment";
            } else if (0 == GetInstrumentType().compare("paymentReceipt")) {
                const Amount lAmount =
                    Opentxs::Client().Exec().StringToLong(m_str_amount);

                if (!IsCanceled() && (lAmount > 0)) strKind.Set("received ");

                if (HasOriginType()) {
                    if (IsOriginTypePaymentPlan()) {
                        str_instrument_type = "recurring payment";
                    } else if (IsOriginTypeSmartContract()) {
                        str_instrument_type = "smart contract payment";
                    } else {
                        str_instrument_type = "automated payment";
                    }
                } else {
                    str_instrument_type = "automated payment";
                }
            }

            strDescription.Format(
                "%s%s%s %s",
                strKind.Get(),
                str_instrument_type.c_str(),
                strTransNumForDisplay.Get(),
                strStatus.Get());
        }  // IsReceipt
        else
            strDescription.Format(
                "%s%s%s %s",
                strKind.Get(),
                GetInstrumentType().c_str(),
                strTransNumForDisplay.Get(),
                strStatus.Get());
    }  // IsRecord
    // ----------------------------------------
    else {
        if (IsTransfer())
            strDescription.Format(
                "%s %s%s%s",
                strStatus.Get(),
                strKind.Get(),
                "transfer",
                strTransNumForDisplay.Get());
        else if (IsVoucher())
            strDescription.Format(
                "%s %s%s%s",
                strStatus.Get(),
                strKind.Get(),
                "payment",
                strTransNumForDisplay.Get());

        else if (IsReceipt()) {
            std::string str_instrument_type;

            if (0 == GetInstrumentType().compare("transferReceipt")) {
                if (IsCanceled())
                    str_instrument_type = "transfer";
                else
                    str_instrument_type = "transfer (receipt)";
            } else if (0 == GetInstrumentType().compare("finalReceipt")) {
                if (HasOriginType()) {
                    if (IsOriginTypeMarketOffer()) {
                        str_instrument_type = "market offer (final receipt)";
                    } else if (IsOriginTypePaymentPlan()) {
                        str_instrument_type = "payment plan (final receipt)";
                    } else if (IsOriginTypeSmartContract()) {
                        str_instrument_type = "smart contract (final receipt)";
                    } else {
                        str_instrument_type = "final receipt";
                    }
                } else {
                    str_instrument_type = "final receipt";
                }
            } else if (0 == GetInstrumentType().compare("marketReceipt")) {
                const Amount lAmount =
                    Opentxs::Client().Exec().StringToLong(m_str_amount);

                // I *think* marketReceipts have negative value. We'll just test
                // for non-zero.
                if (lAmount == 0)
                    str_instrument_type = "failed trade";
                else
                    str_instrument_type = "market trade (receipt)";
            } else if (0 == GetInstrumentType().compare("chequeReceipt")) {
                const Amount lAmount =
                    Opentxs::Client().Exec().StringToLong(m_str_amount);

                // I paid OUT when this chequeReceipt came through. It must be a
                // normal cheque that I wrote.
                if (lAmount <= 0)  // Notice less than OR EQUAL TO 0 -- that's
                                   // because a canceled cheque has a 0 value.
                {
                    if (IsCanceled())
                        str_instrument_type = "cheque";
                    else
                        str_instrument_type = "cheque (receipt)";
                } else  // I GOT PAID when this chequeReceipt came through. It
                        // must be an invoice I wrote, that someone paid.
                {
                    if (IsCanceled())
                        str_instrument_type = "invoice";
                    else
                        str_instrument_type =
                            "sent invoice (receipt: payment received)";
                }
            } else if (0 == GetInstrumentType().compare("voucherReceipt")) {
                if (IsCanceled())
                    str_instrument_type = "payment";
                else
                    str_instrument_type = "payment (receipt)";
            } else if (0 == GetInstrumentType().compare("paymentReceipt")) {
                const Amount lAmount =
                    Opentxs::Client().Exec().StringToLong(m_str_amount);

                if (!IsCanceled() && (lAmount > 0)) strKind.Set("received ");

                if (HasOriginType()) {
                    if (IsOriginTypePaymentPlan()) {
                        str_instrument_type = "recurring payment (receipt)";
                    } else if (IsOriginTypeSmartContract()) {
                        str_instrument_type =
                            "smart contract payment (receipt)";
                    } else {
                        str_instrument_type = "automated payment (receipt)";
                    }
                } else {
                    str_instrument_type = "automated payment (receipt)";
                }
            }

            strDescription.Format(
                "%s %s%s%s",
                strStatus.Get(),
                strKind.Get(),
                str_instrument_type.c_str(),
                strTransNumForDisplay.Get());
        } else
            strDescription.Format(
                "%s %s%s%s",
                strStatus.Get(),
                strKind.Get(),
                GetInstrumentType().c_str(),
                strTransNumForDisplay.Get());
    }  // Not Record.
    // ----------------------------------------
    str_output = strDescription.Get();
    return (!str_output.empty());
}
bool Record::HasInitialPayment() const
{
    if (!IsPaymentPlan()) return false;

    auto thePlan{backlink_.client_.Factory().PaymentPlan(backlink_.client_)};

    OT_ASSERT(false != bool(thePlan));

    const String strPlan(GetContents().c_str());

    if (thePlan->LoadContractFromString(strPlan) &&
        thePlan->HasInitialPayment())
        return true;
    return false;
}

bool Record::HasPaymentPlan() const
{
    if (!IsPaymentPlan()) return false;

    auto thePlan{backlink_.client_.Factory().PaymentPlan(backlink_.client_)};

    OT_ASSERT(false != bool(thePlan));

    const String strPlan(GetContents().c_str());

    if (thePlan->LoadContractFromString(strPlan) && thePlan->HasPaymentPlan())
        return true;
    return false;
}

time64_t Record::GetInitialPaymentDate() const
{
    if (!IsPaymentPlan()) return OT_TIME_ZERO;

    auto thePlan{backlink_.client_.Factory().PaymentPlan(backlink_.client_)};

    OT_ASSERT(false != bool(thePlan));

    const String strPlan(GetContents().c_str());

    if (thePlan->LoadContractFromString(strPlan) &&
        thePlan->HasInitialPayment())
        return thePlan->GetInitialPaymentDate();
    return OT_TIME_ZERO;
}

Amount Record::GetInitialPaymentAmount() const
{
    if (!IsPaymentPlan()) return 0;

    auto thePlan{backlink_.client_.Factory().PaymentPlan(backlink_.client_)};

    OT_ASSERT(false != bool(thePlan));

    const String strPlan(GetContents().c_str());

    if (thePlan->LoadContractFromString(strPlan) &&
        thePlan->HasInitialPayment())
        return thePlan->GetInitialPaymentAmount();
    return 0;
}

time64_t Record::GetPaymentPlanStartDate() const
{
    if (!IsPaymentPlan()) return OT_TIME_ZERO;

    auto thePlan{backlink_.client_.Factory().PaymentPlan(backlink_.client_)};

    OT_ASSERT(false != bool(thePlan));

    const String strPlan(GetContents().c_str());

    if (thePlan->LoadContractFromString(strPlan) && thePlan->HasPaymentPlan())
        return thePlan->GetPaymentPlanStartDate();
    return OT_TIME_ZERO;
}

time64_t Record::GetTimeBetweenPayments() const
{
    if (!IsPaymentPlan()) return OT_TIME_ZERO;

    auto thePlan{backlink_.client_.Factory().PaymentPlan(backlink_.client_)};

    OT_ASSERT(false != bool(thePlan));

    const String strPlan(GetContents().c_str());

    if (thePlan->LoadContractFromString(strPlan) && thePlan->HasPaymentPlan())
        return thePlan->GetTimeBetweenPayments();
    return OT_TIME_ZERO;
}

Amount Record::GetPaymentPlanAmount() const
{
    if (!IsPaymentPlan()) return 0;

    auto thePlan{backlink_.client_.Factory().PaymentPlan(backlink_.client_)};

    OT_ASSERT(false != bool(thePlan));

    const String strPlan(GetContents().c_str());

    if (thePlan->LoadContractFromString(strPlan) && thePlan->HasPaymentPlan())
        return thePlan->GetPaymentPlanAmount();
    return 0;
}

std::int32_t Record::GetMaximumNoPayments() const
{
    if (!IsPaymentPlan()) return 0;

    auto thePlan{backlink_.client_.Factory().PaymentPlan(backlink_.client_)};

    OT_ASSERT(false != bool(thePlan));

    const String strPlan(GetContents().c_str());

    if (thePlan->LoadContractFromString(strPlan) && thePlan->HasPaymentPlan())
        return thePlan->GetMaximumNoPayments();
    return 0;
}
Record::RecordType Record::GetRecordType() const { return m_RecordType; }

bool Record::IsFinalReceipt() const { return m_bIsFinalReceipt; }

void Record::SetFinalReceipt(bool bValue /*=true*/)
{
    m_bIsFinalReceipt = bValue;
}

void Record::SetClosingNum(const TransactionNumber lClosingNum)
{
    m_lClosingNum = lClosingNum;
}

bool Record::GetClosingNum(TransactionNumber& lClosingNum) const
{
    if (!m_bIsFinalReceipt) return false;

    if (0 >= m_lClosingNum) return false;

    lClosingNum = m_lClosingNum;
    return true;
}

// For completed records (not pending.)
//
bool Record::CanDeleteRecord() const
{
    if (IsMail()) return true;

    if (!IsRecord()) return false;

    if (IsPending())  // This may be superfluous given the above 'if'
                      // statement.
        return false;

    return true;
}
// For incoming, pending (not-yet-accepted) instruments.
//
bool Record::CanAcceptIncoming() const
{
    // Commented out because a transferReceipt is in the inbox, but it
    // represents an outgoing payment.
    //    if (IsOutgoing()) // If it's outgoing, then it's definitely not
    // an incoming thing you can accept.
    //        return false;

    if (IsRecord())  // Records must be archived or deleted, not accepted
                     // or discarded.
        return false;

    if (IsExpired()) return false;

    if (IsReceipt())  // It's NOT a record... If it's a receipt, then yes,
                      // we can accept it.
        return true;

    if (IsMail())  // Can't "accept" mail, can only delete it.
        return false;

    if (IsPending() && IsOutgoing())  // It's not a record, it's not
                                      // a receipt. If it's pending,
                                      // is it Outgoing pending? (Can
                                      // only accept INCOMING
                                      // pending, not outgoing.)
        return false;

    return true;
}
// For INcoming, pending (not-yet-accepted) instruments.
//
bool Record::CanDiscardIncoming() const
{
    if (IsOutgoing()) return false;

    if (!IsPending()) return false;

    if (IsMail()) return false;

    if (IsRecord())  // This may be superfluous given the above 'if'
                     // pending.
        return false;

    if (IsReceipt())  // Receipts must be accepted, not discarded.
        return false;

    if (Record::Transfer == GetRecordType())  // All incoming, pending
                                              // instruments EXCEPT
                                              // transfer can be
                                              // discarded.
        return false;

    return true;
}
bool Record::CanDiscardOutgoingCash() const  // For OUTgoing cash. (No way to
                                             // see if it's been accepted, so
                                             // this lets you erase the record
                                             // of sending it.)
{
    if (!IsOutgoing()) return false;

    if (!IsPending()) return false;

    if (!IsCash()) return false;

    if (!(GetBoxIndex() >= 0)) return false;

    return true;
}
// For OUTgoing, pending (not-yet-accepted) instruments.
//
bool Record::CanCancelOutgoing() const
{
    if (!IsOutgoing()) return false;

    if (IsCanceled())  // It's already canceled!
        return false;

    if (!IsPending()) return false;

    if (IsMail()) return false;

    if (IsRecord())  // This may be superfluous given the above 'if'
                     // pending.
        return false;

    if (IsReceipt())  // Receipts can't be canceled. (Probably
                      // superfluous.)
        return false;

    if (Record::Transfer == GetRecordType())  // All outgoing, pending
                                              // instruments EXCEPT transfer
                                              // can be canceled.
        return false;

    return true;
}

bool Record::DiscardOutgoingCash() const
{
    if (!CanDiscardOutgoingCash()) return false;
    return SwigWrap::Nym_RemoveOutpaymentsByIndex(m_str_nym_id, GetBoxIndex());
}

// For completed records (not pending.)
//
bool Record::DeleteRecord() const
{
    if (!CanDeleteRecord()) return false;
    // ----------------------------------------------
    std::string str_using_account;
    bool bUsingAccountOrNym{false};

    if ((Record::Transfer == GetRecordType()) ||
        (Record::Receipt == GetRecordType())) {
        if (m_str_account_id.empty()) {
            otErr << __FUNCTION__
                  << ": Error: missing account id for transfer or receipt.\n";
            return false;
        }
        bUsingAccountOrNym = true;
        str_using_account =
            m_str_account_id;  // For receipts in asset acct box.
    } else {
        bUsingAccountOrNym = false;
        str_using_account = m_str_nym_id;  // For instruments in payment box.
    }
    // ----------------------------------------------
    if (!m_bIsSpecialMail) {
        if (m_str_nym_id.empty()) {
            otErr << __FUNCTION__ << ": Error: missing nym id (" << m_str_nym_id
                  << ")\n";
            return false;
        }
        if (bUsingAccountOrNym && m_str_pmnt_notary_id.empty()) {
            otErr << __FUNCTION__ << ": Error: missing pmnt notary id ("
                  << m_str_pmnt_notary_id << ")\n";
            return false;
        }
        if (!bUsingAccountOrNym && m_str_msg_notary_id.empty()) {
            otErr << __FUNCTION__ << ": Error: missing msg notary id ("
                  << m_str_msg_notary_id << ")\n";
            return false;
        }
    }
    // ----------------------------------------------
    switch (GetRecordType()) {
        // Delete from in-mail or out-mail.
        //
        case Record::Mail: {
            // If it's a Bitmessage or any other "non-OT" mail, then we can't
            // delete it here. We have to assume the client application is
            // smart enough to check for this and perform the deletion itself.
            // (Not all client apps will even support forms of mail other than
            // OT mail.)
            //
            if (!m_bIsSpecialMail) {
                if (m_bIsOutgoing)  // outgoing mail
                {
                    auto& exec = Opentxs::Client().Exec();
                    const auto list = exec.GetNym_OutmailCount(m_str_nym_id);

                    for (const auto& id : list) {
                        const auto mail = exec.GetNym_OutmailContentsByIndex(
                            m_str_nym_id, id);

                        if (mail == m_str_contents) {

                            return exec.Nym_RemoveOutmailByIndex(
                                m_str_nym_id, id);
                        }
                    }
                } else  // incoming mail
                {
                    auto& exec = Opentxs::Client().Exec();
                    const auto list = exec.GetNym_MailCount(m_str_nym_id);

                    for (const auto& id : list) {
                        const auto mail =
                            exec.GetNym_MailContentsByIndex(m_str_nym_id, id);

                        if (mail == m_str_contents) {

                            return exec.Nym_RemoveMailByIndex(m_str_nym_id, id);
                        }
                    }
                }
            } else
                return true;

            return false;
        } break;
        case Record::Transfer:    // Delete from asset account recordbox.
        case Record::Receipt:     // Delete from asset account recordbox.
        case Record::Instrument:  // Delete from Nym's recordbox.
        case Record::Notice:      // Delete from Nym's recordbox.
            break;
        default:
            otErr << __FUNCTION__
                  << ": Unexpected type: " << GetInstrumentType() << "\n";
            return false;
    }
    // The below section handles both the Nym's recordbox AND the Asset Account
    // recordbox.
    //
    if (0 == m_lTransactionNum) {
        otErr << __FUNCTION__
              << ": Error: Transaction number is 0, in recordbox for "
                 "msg notary id ("
              << m_str_msg_notary_id << "), pmnt notary id ("
              << m_str_pmnt_notary_id << "), nym id (" << m_str_nym_id
              << "), acct id (" << str_using_account << ")\n";
        return false;
    }

    // Sometimes this is an asset account record box for the payment notary.
    // Sometimes this is a Nym's payments record box for the transport notary.
    //
    // In the first case, it's a transfer or receipt that was in your asset
    // account inbox and is now in the corresponding record box. That's the
    // account on the notary where the payment actually happened.
    //
    // In the second case, it's a cheque that someone sent you, which was in
    // your payments inbox (on the server where you receive MESSAGES, NOT on
    // the server where that cheque was drawn!) Now since you already deposited
    // it or whatever, it's now in the record box that corresponds to that
    // payments inbox on your transport notary.
    //
    // So in some cases, we want to use your payment notary, and in other cases,
    // we want to use your transport notary.

    const auto theNotaryID = Identifier::Factory(
                   bUsingAccountOrNym ? m_str_pmnt_notary_id
                                      : m_str_msg_notary_id),
               theNymID = Identifier::Factory(m_str_nym_id),
               theAcctID =
                   Identifier::Factory(str_using_account);  // this last one
                                                            // sometimes
                                                            // contains NymID
                                                            // (see above.)

    auto pRecordbox = Opentxs::Client().OTAPI().LoadRecordBox(
        theNotaryID, theNymID, theAcctID);
    if (false == bool(pRecordbox)) {
        otErr << __FUNCTION__
              << ": Failed loading record box for msg notary ID"
                 " ("
              << m_str_msg_notary_id << ") nymID (" << m_str_nym_id
              << ") accountID (" << str_using_account << ")\n";
        return false;
    }
    // Find the receipt in the recordbox that correlates to this Record.
    //
    int nIndex = pRecordbox->GetTransactionIndex(m_lTransactionNum);

    if ((-1) == nIndex) {
        otErr << __FUNCTION__ << ": Error: Unable to find transaction "
              << m_lTransactionNum
              << " in recordbox "
                 "for server id ("
              << theNotaryID->str() << "), nym id (" << m_str_nym_id
              << "), acct id (" << str_using_account << ")\n";
        return false;
    }
    // Accept it.
    //
    return SwigWrap::ClearRecord(
        // m_str_msg_notary_id,
        theNotaryID->str(),
        m_str_nym_id,
        str_using_account,
        nIndex,
        false);  // clear all = false. We're only clearing one record.
}

bool Record::accept_inbox_items(
    const std::string& ACCOUNT_ID,
    std::int32_t nItemType,
    const std::string& INDICES) const
{
    //  enum ItemType { typeBoth = 0, typeTransfers = 1, typeReceipts = 2 };
    switch (nItemType) {
        case 0: {
            return 1 == RecordList::acceptFromInbox(
                            ACCOUNT_ID, INDICES, RecordList::typeBoth);
        }

        case 1: {
            return 1 == RecordList::acceptFromInbox(
                            ACCOUNT_ID, INDICES, RecordList::typeTransfers);
        }

        case 2: {
            return 1 == RecordList::acceptFromInbox(
                            ACCOUNT_ID, INDICES, RecordList::typeReceipts);
        }

        default:
            otErr << __FUNCTION__ << ": Invalid nItemType.\n";
            break;
    }

    return false;
}

bool Record::discard_incoming_payments(
    const std::string& TRANSPORT_NOTARY_ID,
    const std::string& NYM_ID,
    const std::string& INDICES) const
{
    return 1 == RecordList::discard_incoming_payments(
                    TRANSPORT_NOTARY_ID, NYM_ID, INDICES);
}

bool Record::cancel_outgoing_payments(
    const std::string& nymID,
    const std::string& ACCOUNT_ID,
    const std::string& INDICES) const
{
    return 1 ==
           RecordList::cancel_outgoing_payments(nymID, ACCOUNT_ID, INDICES);
}

bool Record::AcceptIncomingTransfer() const
{
    return AcceptIncomingTransferOrReceipt();
}  // For incoming, pending (not-yet-accepted) transfers.
bool Record::AcceptIncomingReceipt() const
{
    return AcceptIncomingTransferOrReceipt();
}  // For incoming, (not-yet-accepted) receipts.
bool Record::AcceptIncomingTransferOrReceipt() const
{
    if (!CanAcceptIncoming()) return false;

    switch (GetRecordType()) {
        // Accept transfer or receipt from asset account inbox.
        //
        case Record::Transfer:
        case Record::Receipt: {
            if (m_str_pmnt_notary_id.empty() || m_str_nym_id.empty() ||
                m_str_account_id.empty()) {
                otErr << __FUNCTION__ << ": Error: missing pmnt notary id ("
                      << m_str_pmnt_notary_id << ") or nym id (" << m_str_nym_id
                      << ") or "
                         "acct id ("
                      << m_str_account_id << ")\n";
                return false;
            }
            if (0 == m_lTransactionNum) {
                otErr << __FUNCTION__
                      << ": Error: Transaction number is 0, in asset "
                         "account inbox for pmnt notary id ("
                      << m_str_pmnt_notary_id << "), nym id (" << m_str_nym_id
                      << ")\n";
                return false;
            }
            const auto thePmntNotaryID =
                           Identifier::Factory(m_str_pmnt_notary_id),
                       theNymID = Identifier::Factory(m_str_nym_id),
                       theAcctID = Identifier::Factory(m_str_account_id);

            // Open the Nym's asset account inbox.
            auto pInbox = Opentxs::Client().OTAPI().LoadInbox(
                thePmntNotaryID, theNymID, theAcctID);
            if (false == bool(pInbox)) {
                otErr << __FUNCTION__
                      << ": Error: Unable to load asset account inbox for "
                         "pmnt notary id ("
                      << m_str_pmnt_notary_id << "), nym id (" << m_str_nym_id
                      << ")\n";
                return false;
            }
            // Find the transfer/receipt therein that correlates to this
            // Record.
            //
            int nIndex = pInbox->GetTransactionIndex(m_lTransactionNum);

            if ((-1) == nIndex) {
                otErr << __FUNCTION__ << ": Error: Unable to find transaction "
                      << m_lTransactionNum
                      << " in asset account inbox "
                         "for pmnt notary id ("
                      << m_str_pmnt_notary_id << "), nym id (" << m_str_nym_id
                      << "), acct id (" << m_str_account_id << ")\n";
                return false;
            }
            // Accept it.
            //
            String strIndices;
            strIndices.Format("%d", nIndex);
            const std::string str_indices(strIndices.Get());

            return accept_inbox_items(m_str_account_id, 0, str_indices);
        } break;
        default:
            otErr << __FUNCTION__
                  << ": Unexpected type: " << GetInstrumentType() << "\n";
            return false;
    }

    return true;
}
// For incoming, pending (not-yet-accepted) instruments.
//
bool Record::AcceptIncomingInstrument(const std::string& str_into_acct) const
{
    if (!CanAcceptIncoming()) return false;

    switch (GetRecordType()) {
        // Accept from Nym's payments inbox.
        //
        case Record::Instrument: {
            if (m_str_msg_notary_id.empty() || m_str_nym_id.empty()) {
                otErr << __FUNCTION__ << ": Error: missing msg notary id ("
                      << m_str_msg_notary_id << ") or nym id (" << m_str_nym_id
                      << ")\n";
                return false;
            }
            if (0 == m_lTransactionNum) {
                otErr << __FUNCTION__
                      << ": Error: Transaction number is 0, in payment "
                         "inbox for msg notary id ("
                      << m_str_msg_notary_id << "), nym id (" << m_str_nym_id
                      << ")\n";
                return false;
            }
            const auto theMsgNotaryID =
                           Identifier::Factory(m_str_msg_notary_id),
                       theNymID = Identifier::Factory(m_str_nym_id);

            // Open the Nym's payments inbox.
            auto pInbox = Opentxs::Client().OTAPI().LoadPaymentInbox(
                theMsgNotaryID, theNymID);
            if (!pInbox) {
                otErr << __FUNCTION__
                      << ": Error: Unable to load payment inbox for msg notary "
                         "id ("
                      << m_str_msg_notary_id << "), nym id (" << m_str_nym_id
                      << ")\n";
                return false;
            }
            // Find the payment therein that correlates to this Record.
            //
            int nIndex = pInbox->GetTransactionIndex(m_lTransactionNum);

            if ((-1) == nIndex) {
                otErr << __FUNCTION__ << ": Error: Unable to find transaction "
                      << m_lTransactionNum
                      << " in "
                         "payment inbox for transport notary id ("
                      << m_str_msg_notary_id << "), nym id (" << m_str_nym_id
                      << ")\n";
                return false;
            }
            // Accept it.
            //
            String strIndices;
            strIndices.Format("%d", nIndex);
            const std::string str_indices(strIndices.Get());

            const char* szPaymentType =
                IsPaymentPlan() ? "PAYMENT PLAN" : "ANY";

            // NOTE: Someday if the command line tool ever starts using
            // RecordList, then we will actually remove the block below, and
            // ALLOW smart contracts to be activated here. That's because we can
            // just allow the user to enter confirmSmartContract which ALREADY
            // expects that it needs to ask the user to provide various Nym and
            // Account Ids during the confirmation process AT THE COMMAND LINE.
            // But right now, only GUIs use RecordList, and you don't want the
            // GUI to hang up while the application is suddenly asking you
            // questions on a command line terminal somewhere!
            if (IsContract()) {
                otErr << __FUNCTION__
                      << ": It's a bug that this function was even called in "
                         "the first place! "
                         "The reason is because you CANNOT confirm a smart "
                         "contract here. The confirmation process requires "
                         "a whole wizard so the user can select various Nyms "
                         "and Accounts. The GUI must perform this process, and "
                         "thus would do that instead of ever calling this "
                         "function. (So why are we here again...?)\n";
                return false;
            }

            std::string str_server_response;
            if (!RecordList::accept_from_paymentbox(
                    m_str_msg_notary_id,
                    str_into_acct,
                    str_indices,
                    szPaymentType,
                    &str_server_response)) {
                otErr << __FUNCTION__
                      << ": Error while trying to accept this instrument.\n";
                return false;
            } else
                backlink_.notifyOfSuccessfulNotarization(
                    str_into_acct,
                    m_str_nym_id,
                    m_str_msg_notary_id,
                    m_str_pmnt_notary_id,
                    str_server_response,
                    m_lTransactionNum,
                    m_lTransNumForDisplay);
        }  // case: instrument
        break;
        default:
            otErr << __FUNCTION__
                  << ": Unexpected type: " << GetInstrumentType() << "\n";
            return false;
    }

    return true;
}

// For incoming, pending (not-yet-accepted) instruments.
//
bool Record::DiscardIncoming() const
{
    if (!CanDiscardIncoming()) return false;

    switch (GetRecordType()) {
        case Record::Instrument: {
            if (m_str_msg_notary_id.empty() || m_str_nym_id.empty()) {
                otErr << __FUNCTION__ << ": Error: missing server id ("
                      << m_str_msg_notary_id << ") or nym id (" << m_str_nym_id
                      << ")\n";
                return false;
            }
            if (0 == m_lTransactionNum) {
                otErr << __FUNCTION__
                      << ": Error: Transaction number is 0, in "
                         "payment inbox for transport notary id ("
                      << m_str_msg_notary_id << "), nym id (" << m_str_nym_id
                      << ")\n";
                return false;
            }
            const auto theMsgNotaryID =
                           Identifier::Factory(m_str_msg_notary_id),
                       theNymID = Identifier::Factory(m_str_nym_id);

            // Open the Nym's payments inbox.
            auto pInbox = Opentxs::Client().OTAPI().LoadPaymentInbox(
                theMsgNotaryID, theNymID);
            if (!pInbox) {
                otErr << __FUNCTION__
                      << ": Error: Unable to load payment inbox for transport "
                         "notary id ("
                      << m_str_msg_notary_id << "), nym id (" << m_str_nym_id
                      << ")\n";
                return false;
            }
            // Find the payment therein that correlates to this Record.
            //
            int nIndex = pInbox->GetTransactionIndex(m_lTransactionNum);

            if ((-1) == nIndex) {
                otErr << __FUNCTION__ << ": Error: Unable to find transaction "
                      << m_lTransactionNum
                      << " in "
                         "payment inbox for transport notary id ("
                      << m_str_msg_notary_id << "), nym id (" << m_str_nym_id
                      << ")\n";
                return false;
            }
            // Accept it.
            //
            String strIndices;
            strIndices.Format("%d", nIndex);
            const std::string str_indices(strIndices.Get());

            return discard_incoming_payments(
                m_str_msg_notary_id, m_str_nym_id, str_indices);

        }  // case: instrument
        break;
        default:
            otErr << __FUNCTION__
                  << ": Unexpected type: " << GetInstrumentType() << "\n";
            return false;
    }

    return true;
}
bool Record::IsTransfer() const { return (Record::Transfer == m_RecordType); }
bool Record::IsCash() const { return m_bIsCash; }
bool Record::IsNotice() const { return m_bIsNotice; }
bool Record::IsInvoice() const { return m_bIsInvoice; }
bool Record::IsCheque() const { return m_bIsCheque; }
bool Record::IsVoucher() const { return m_bIsVoucher; }
bool Record::IsContract() const { return m_bIsSmartContract; }
bool Record::IsPaymentPlan() const { return m_bIsPaymentPlan; }

void Record::SetSuccess(const bool bIsSuccess)
{
    m_bHasSuccess = true;
    m_bIsSuccess = bIsSuccess;
}

bool Record::HasSuccess(bool& bIsSuccess) const
{
    if (m_bHasSuccess) bIsSuccess = m_bIsSuccess;

    return m_bHasSuccess;
}

// For outgoing, pending (not-yet-accepted) instruments.
//
bool Record::CancelOutgoing(std::string str_via_acct) const  // This can be
                                                             // blank if it's
                                                             // a cheque.
{
    if (!CanCancelOutgoing()) return false;

    switch (GetRecordType()) {
        case Record::Instrument: {
            if (m_str_nym_id.empty()) {
                otErr << __FUNCTION__ << ": Error: missing nym id ("
                      << m_str_nym_id << ")\n";
                return false;
            }

            const auto theNymID = Identifier::Factory(m_str_nym_id);
            std::string str_using_acct;

            if (IsCheque()) {
                str_using_acct = m_str_account_id;
            } else {
                str_using_acct = str_via_acct;
            }
            if (str_using_acct.empty()) {
                otErr << __FUNCTION__
                      << ": Error: Missing account ID (FAILURE)\n";
                return false;
            }
            if (0 == m_lTransactionNum) {
                if (IsCash()) {
                    // Maybe it's cash...
                    std::string strOutpayment(
                        SwigWrap::GetNym_OutpaymentsContentsByIndex(
                            m_str_nym_id, GetBoxIndex()));

                    if (strOutpayment.empty()) {
                        std::int32_t lIndex = GetBoxIndex();
                        otErr << __FUNCTION__
                              << ": Error: Blank outpayment at index " << lIndex
                              << "\n";
                        return false;
                    }
                    String strPayment(strOutpayment);
                    auto thePayment{Opentxs::Client().Factory().Payment(
                        Opentxs::Client(), strPayment)};

                    OT_ASSERT(false != bool(thePayment));

                    if (!thePayment->IsValid() ||
                        !thePayment->SetTempValues()) {
                        std::int32_t lIndex = GetBoxIndex();
                        otErr << __FUNCTION__
                              << ": Error: Invalid outpayment at index "
                              << lIndex << "\n";
                        return false;
                    }
                    TransactionNumber lTransNum = 0;
                    thePayment->GetOpeningNum(lTransNum, theNymID);
                    if (0 == lTransNum)  // Found it.
                    {
                        std::int32_t lIndex = GetBoxIndex();
                        String strIndices;
                        strIndices.Format("%d", lIndex);
                        const std::string str_indices(strIndices.Get());

                        return cancel_outgoing_payments(
                            m_str_nym_id, str_using_acct, str_indices);
                    } else {
                        otErr << __FUNCTION__
                              << ": Error: Transaction number is non-zero ("
                              << lTransNum
                              << "), for cash outgoing payment for nym id ("
                              << m_str_nym_id << ")\n";
                        return false;
                    }
                }  // IsCash()
                else {
                    otErr << __FUNCTION__
                          << ": Error: Transaction number is 0, "
                             "for non-cash outgoing payment for "
                             "nym id ("
                          << m_str_nym_id << ")\n";
                    return false;
                }
            }
            // Find the payment in the Nym's outpayments box that correlates to
            // this Record.
            //
            std::int32_t nCount =
                SwigWrap::GetNym_OutpaymentsCount(m_str_nym_id);

            for (std::int32_t nIndex = 0; nIndex < nCount; ++nIndex) {
                std::string strOutpayment(
                    SwigWrap::GetNym_OutpaymentsContentsByIndex(
                        m_str_nym_id, nIndex));

                if (strOutpayment.empty()) {
                    otErr << __FUNCTION__
                          << ": Error: Blank outpayment at index " << nIndex
                          << "\n";
                    return false;
                }
                String strPayment(strOutpayment);
                auto thePayment{Opentxs::Client().Factory().Payment(
                    Opentxs::Client(), strPayment)};

                OT_ASSERT(false != bool(thePayment));

                if (!thePayment->IsValid() || !thePayment->SetTempValues()) {
                    otErr << __FUNCTION__
                          << ": Error: Invalid outpayment at index " << nIndex
                          << "\n";
                    return false;
                }
                TransactionNumber lTransNum = 0;
                thePayment->GetOpeningNum(lTransNum, theNymID);
                if (lTransNum == m_lTransactionNum)  // Found it.
                {
                    String strIndices;
                    strIndices.Format("%d", nIndex);
                    const std::string str_indices(strIndices.Get());

                    return cancel_outgoing_payments(
                        m_str_nym_id, str_using_acct, str_indices);
                }
            }  // for
        } break;
        default:
            otErr << __FUNCTION__
                  << ": Unexpected type: " << GetInstrumentType() << "\n";
            return false;
    }

    return true;
}
TransactionNumber Record::GetTransactionNum() const
{
    return m_lTransactionNum;
}  // Trans Num of receipt in the box. (Unless outpayment, contains number for
   // instrument.)
void Record::SetTransactionNum(TransactionNumber lTransNum)
{
    m_lTransactionNum = lTransNum;
}
TransactionNumber Record::GetTransNumForDisplay() const
{
    if (m_lTransNumForDisplay > 0) return m_lTransNumForDisplay;

    return m_lTransactionNum;
}
void Record::SetTransNumForDisplay(TransactionNumber lTransNum)
{
    m_lTransNumForDisplay = lTransNum;
}
void Record::SetExpired() { m_bIsExpired = true; }
void Record::SetCanceled() { m_bIsCanceled = true; }
bool Record::IsMail() const { return Record::Mail == GetRecordType(); }
bool Record::IsPending() const { return m_bIsPending; }
bool Record::IsOutgoing() const { return m_bIsOutgoing; }
bool Record::IsRecord() const { return m_bIsRecord; }
bool Record::IsReceipt() const { return m_bIsReceipt; }
bool Record::HasContents() const { return !m_str_contents.empty(); }
bool Record::HasMemo() const { return !m_str_memo.empty(); }
bool Record::IsExpired() const { return m_bIsExpired; }
bool Record::IsCanceled() const { return m_bIsCanceled; }
const std::string& Record::GetMsgNotaryID() const
{
    return m_str_msg_notary_id;
}
const std::string& Record::GetPmntNotaryID() const
{
    return m_str_pmnt_notary_id;
}

const std::string& Record::GetUnitTypeID() const { return m_str_unit_type_id; }
const std::string& Record::GetCurrencyTLA() const { return m_str_currency_tla; }
const std::string& Record::GetNymID() const { return m_str_nym_id; }
const std::string& Record::GetAccountID() const { return m_str_account_id; }
const std::string& Record::GetOtherNymID() const { return m_str_other_nym_id; }
const std::string& Record::GetOtherAccountID() const
{
    return m_str_other_account_id;
}
const std::string& Record::GetName() const { return m_str_name; }
const std::string& Record::GetDate() const { return m_str_date; }
const std::string& Record::GetAmount() const { return m_str_amount; }
const std::string& Record::GetInstrumentType() const { return m_str_type; }
const std::string& Record::GetMemo() const { return m_str_memo; }
const std::string& Record::GetContents() const { return m_str_contents; }
bool Record::IsSpecialMail() const { return m_bIsSpecialMail; }
std::int32_t Record::GetMethodID() const { return m_nMethodID; }
const std::string& Record::GetAddress() const { return m_str_my_address; }
const std::string& Record::GetOtherAddress() const
{
    return m_str_other_address;
}
const std::string& Record::GetMsgID() const { return m_str_msg_id; }
const std::string& Record::GetMsgType() const { return m_str_msg_type; }
const std::string& Record::GetMsgTypeDisplay() const
{
    return m_str_msg_type_display;
}
void Record::SetSpecialMail(bool bIsSpecial) { m_bIsSpecialMail = bIsSpecial; }
void Record::SetMethodID(std::int32_t nMethodID) { m_nMethodID = nMethodID; }
void Record::SetAddress(const std::string& str_Address)
{
    m_str_my_address = str_Address;
}
void Record::SetOtherAddress(const std::string& str_Address)
{
    m_str_other_address = str_Address;
}
void Record::SetMsgID(const std::string& str_id) { m_str_msg_id = str_id; }
void Record::SetMsgType(const std::string& str_type)
{
    m_str_msg_type = str_type;
}
void Record::SetMsgTypeDisplay(const std::string& str_type)
{
    m_str_msg_type_display = str_type;
}
std::int32_t Record::GetBoxIndex() const { return m_nBoxIndex; }
void Record::SetBoxIndex(std::int32_t nBoxIndex) { m_nBoxIndex = nBoxIndex; }
const std::string Record::GetThreadItemId() const { return m_strThreadItemId; }
void Record::SetThreadItemId(const std::string& strThreadItemId)
{
    m_strThreadItemId = strThreadItemId;
}
void Record::SetMemo(const std::string& str_memo) { m_str_memo = str_memo; }
void Record::SetOtherNymID(const std::string& str_ID)
{
    m_str_other_nym_id = str_ID;
}
void Record::SetOtherAccountID(const std::string& str_ID)
{
    m_str_other_account_id = str_ID;
}
void Record::SetContents(const std::string& str_contents)
{
    m_str_contents = str_contents;

    if (!m_str_contents.empty() && ((Record::Instrument == GetRecordType()) ||
                                    (Record::Notice == GetRecordType()))) {
        String strPayment(m_str_contents);
        auto thePayment{
            Opentxs::Client().Factory().Payment(Opentxs::Client(), strPayment)};

        OT_ASSERT(false != bool(thePayment));

        if (thePayment->IsValid() && thePayment->SetTempValues()) {
            switch (thePayment->GetType()) {
                case OTPayment::PURSE:
                    m_bIsCash = true;
                    break;
                case OTPayment::CHEQUE:
                    m_bIsCheque = true;
                    break;
                case OTPayment::VOUCHER:
                    m_bIsVoucher = true;
                    break;
                case OTPayment::INVOICE:
                    m_bIsInvoice = true;
                    break;
                case OTPayment::PAYMENT_PLAN:
                    m_bIsPaymentPlan = true;
                    break;
                case OTPayment::SMART_CONTRACT:
                    m_bIsSmartContract = true;
                    break;
                case OTPayment::NOTICE:
                    m_bIsNotice = true;
                    break;

                default:
                    break;
            }
        }
    }
}
time64_t Record::GetValidFrom() const { return m_ValidFrom; }
time64_t Record::GetValidTo() const { return m_ValidTo; }
void Record::SetDateRange(time64_t tValidFrom, time64_t tValidTo)
{
    m_ValidFrom = tValidFrom;
    m_ValidTo = tValidTo;
    time64_t tCurrentTime = SwigWrap::GetTime();
    if ((tValidTo > OT_TIME_ZERO) && (tCurrentTime > tValidTo) && !IsMail() &&
        !IsRecord())
        SetExpired();
}

// enum class originType : std::int8_t {
//    not_applicable,
//    origin_market_offer, // finalReceipt
//    origin_payment_plan, // finalReceipt, paymentReceipt
//    origin_smart_contract, // finalReceipt, paymentReceipt
//    origin_pay_dividend, // SOME voucherReceipts are from a payDividend.
//    origin_error_state
//};
// Only a few receipts (finalReceipts and paymentReceipts currently)
// even have an origin type set. (Usually it's already known and obvious.)
// But in a few cases, we need to know for example if the finalReceipt
// came from a market offer, a payment plan, or a smart contract.
//
// Make SURE you check HasOriginType first, since the answer given by
// the other functions may otherwise seem intuitively wrong.
//
bool Record::HasOriginType() const { return m_bHasOriginType; }

bool Record::IsOriginTypeMarketOffer() const
{
    return m_bHasOriginType &&
           (originType::origin_market_offer == m_originType);
}

bool Record::IsOriginTypePaymentPlan() const
{
    return m_bHasOriginType &&
           (originType::origin_payment_plan == m_originType);
}

bool Record::IsOriginTypeSmartContract() const
{
    return m_bHasOriginType &&
           (originType::origin_smart_contract == m_originType);
}

// bool m_bHasOriginType{false};
// originType m_originType{originType::not_applicable};

void Record::SetOriginType(originType theOriginType)
{
    m_originType = theOriginType;

    if ((originType::not_applicable == theOriginType) ||
        (originType::origin_error_state == theOriginType)) {
        m_bHasOriginType = false;
    } else {
        m_bHasOriginType = true;
    }
}

// For sorting purposes.
bool Record::operator<(const Record& rhs)
{
    return m_ValidFrom < rhs.m_ValidFrom;
}
Record::Record(
    RecordList& backlink,
    const std::string& str_msg_notary_id,
    const std::string& str_pmnt_notary_id,
    const std::string& str_instrument_definition_id,
    const std::string& str_currency_tla,
    const std::string& str_nym_id,
    const std::string& str_account_id,
    const std::string& str_name,
    const std::string& str_date,
    const std::string& str_amount,
    const std::string& str_type,
    bool bIsPending,
    bool bIsOutgoing,
    bool bIsRecord,
    bool bIsReceipt,
    RecordType eRecordType)

    : backlink_(backlink)
    , m_ValidFrom(OT_TIME_ZERO)
    , m_ValidTo(OT_TIME_ZERO)
    , m_str_msg_notary_id(str_msg_notary_id)
    , m_str_pmnt_notary_id(str_pmnt_notary_id)
    , m_str_unit_type_id(str_instrument_definition_id)
    , m_str_currency_tla(str_currency_tla)
    , m_str_nym_id(str_nym_id)
    , m_str_account_id(str_account_id)
    , m_str_name(str_name)
    , m_str_date(str_date)
    , m_str_amount(str_amount)
    , m_str_type(str_type)
    , m_bIsPending(bIsPending)
    , m_bIsOutgoing(bIsOutgoing)
    , m_bIsRecord(bIsRecord)
    , m_bIsReceipt(bIsReceipt)
    , m_RecordType(eRecordType)
{
}

}  // namespace opentxs::cli
