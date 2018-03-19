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

#include "CmdBaseInstrument.hpp"

#include <opentxs/api/client/ServerAction.hpp>
#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/client/ServerAction.hpp>
#include <opentxs/client/SwigWrap.hpp>
#include <opentxs/client/Utility.hpp>
#include <opentxs/core/util/Common.hpp>
#include <opentxs/core/Identifier.hpp>
#include <opentxs/core/Log.hpp>
#include <opentxs/ext/OTPayment.hpp>
#include <opentxs/OT.hpp>

#include <stdint.h>
#include <ostream>
#include <string>
#include <vector>

using namespace opentxs;
using namespace std;

CmdBaseInstrument::CmdBaseInstrument() {}

CmdBaseInstrument::~CmdBaseInstrument() {}

int32_t CmdBaseInstrument::getTokens(
    vector<string>& tokens,
    const string& server,
    const string& mynym,
    const string& assetType,
    string purse,
    const string& indices) const
{
#if OT_CASH
    if ("" == indices) {
        return 1;
    }

    int32_t items = SwigWrap::Purse_Count(server, assetType, purse);
    if (0 > items) {
        otOut << "Error: cannot load purse item count.\n";
        return -1;
    }

    if (1 > items) {
        otOut << "Error: The purse is empty.\n";
        return -1;
    }

    bool all = "all" == indices;
    for (int32_t i = 0; i < items; i++) {
        string token = SwigWrap::Purse_Peek(server, assetType, mynym, purse);
        if ("" == token) {
            otOut << "Error:cannot load token from purse.\n";
            return -1;
        }

        purse = SwigWrap::Purse_Pop(server, assetType, mynym, purse);
        if ("" == purse) {
            otOut << "Error: cannot load updated purse.\n";
            return -1;
        }

        string tokenID = SwigWrap::Token_GetID(server, assetType, token);
        if ("" == tokenID) {
            otOut << "Error: cannot get token ID.\n";
            return -1;
        }

        if (!all && SwigWrap::NumList_VerifyQuery(indices, to_string(i))) {
            tokens.push_back(tokenID);
        }
    }

    return 1;
#else
    return -1;
#endif  // OT_CASH
}

#if OT_CASH
// processCashPurse pops the selected tokens off of oldPurse, changes their
// owner to recipientNymID,
// and pushes them onto newPurse, owned by recipientNymID. Meanwhile any
// unselected tokens are pushed
// onto newPurseUnselectedTokens, owned by nymID, which is saved to local
// storage (overwriting...)
// newPurse is returned from this function.
//
// Thus, for example, if there's a problem depositing/exporting/exchanging
// newPurse, then it needs
// to be merged back into the old purse again, which is otherwise still missing
// those tokens. (We
// should keep a copy of them in the payments outbox or something like that, in
// the meantime.)
//
// What is newPurseForSender for?
// Sometimes when processCashPurse is called, a Nym is just removing tokens from
// his purse so he can
// deposit them into his own account. Meaning the "sender" Nym (who owns the
// purse) is the same as the
// "recipient" Nym (who owns the account.) In which case, newPurse (for
// recipient) and newPurseForSender
// will contain the exact same thing. But in the case where they are different
// Nyms (like if I am
// exporting these tokens from my purse in order to give them to someone else)
// then I will want a copy
// of the exported purse for the recipient, and a copy of the same exported
// purse for the sender.
// processCashPurse returns both of those for me. The reason is because
// otherwise, if I just encrypted
// some of my cash tokens to some external Nym, then only THAT Nym's private key
// will ever open them up
// again. Even if I have them in my outpayments box, I can never recover them
// because I can never again
// decrypt them. Therefore I make a special copy of the outgoing purse which is
// encrypted to my own key,
// and put that into my outpayments box instead of the one I sent to the
// recipient (or including both
// of them.) That way the cash can still be recovered if necessary, from my
// outpayments box, and re-imported
// back into my cash purse again.
//
bool CmdBaseInstrument::processCashPurse(
    std::string& newPurse,
    std::string& newPurseForSender,
    const std::string& notaryID,
    const std::string& instrumentDefinitionID,
    const std::string& nymID,
    std::string& oldPurse,
    const std::vector<std::string>& selectedTokens,
    const std::string& recipientNymID,
    bool bPWProtectOldPurse,
    bool bPWProtectNewPurse) const
{
    // By this point, we know that "selected tokens" has a size of 0, or MORE
    // THAN ONE. (But NOT 1 exactly.)
    // (At least, if this function was called by exportCashPurse.)
    std::string strLocation = "OT_ME_processCashPurse";

    // This block handles cases where NO TOKENS ARE SELECTED.
    //
    // (Meaning: "PROCESS" THEM ALL.)
    //
    if (selectedTokens.size() < 1) {
        // newPurse is created, OWNED BY RECIPIENT.
        //
        newPurse =
            (bPWProtectNewPurse ? SwigWrap::CreatePurse_Passphrase(
                                      notaryID, instrumentDefinitionID, nymID)
                                : SwigWrap::CreatePurse(
                                      notaryID,
                                      instrumentDefinitionID,
                                      recipientNymID,
                                      nymID));  // recipientNymID is owner,
                                                // nymID is signer;

        if (!VerifyStringVal(newPurse)) {
            otOut << strLocation << ": "
                  << (bPWProtectNewPurse ? "OT_API_CreatePurse_Passphrase"
                                         : "OT_API_CreatePurse")
                  << " returned null\n";
            return false;
        }

        // We'll create an extra copy of the newPurse, which is encrypted to the
        // sender (instead of the recipient or
        // some passphrase.) We'll call it newPurseForSender. This way the
        // sender can later have the option to recover
        // the cash from his outbox.
        //
        newPurseForSender = SwigWrap::CreatePurse(
            notaryID,
            instrumentDefinitionID,
            nymID,
            nymID);  // nymID is owner, nymID is signer;

        if (!VerifyStringVal(newPurseForSender)) {
            otOut << strLocation
                  << ": Failure: OT_API_CreatePurse returned null\n";
            return false;
        }

        // Iterate through the OLD PURSE. (as tempOldPurse.)
        //
        std::int32_t count =
            SwigWrap::Purse_Count(notaryID, instrumentDefinitionID, oldPurse);
        std::string tempOldPurse = oldPurse;

        for (std::int32_t i = 0; i < count; ++i) {
            // Peek into TOKEN, from the top token on the stack. (And it's STILL
            // on top after this call.)
            //
            std::string token = SwigWrap::Purse_Peek(
                notaryID, instrumentDefinitionID, nymID, tempOldPurse);

            // Now pop the token off of tempOldPurse (our iterator for the old
            // purse).
            // Store updated copy of purse (sans token) into "str1".
            //
            std::string str1 = SwigWrap::Purse_Pop(
                notaryID, instrumentDefinitionID, nymID, tempOldPurse);

            if (!VerifyStringVal(token) || !VerifyStringVal(str1)) {
                otOut << strLocation
                      << ": OT_API_Purse_Peek or OT_API_Purse_Pop "
                         "returned null... SHOULD NEVER HAPPEN. "
                         "Returning null.\n";
                return false;
            }

            // Since pop succeeded, copy the output to tempOldPurse
            // (for next iteration, in case any continues happen below.)
            // Now tempOldPurse contains what it did before, MINUS ONE TOKEN.
            // (The exported one.)
            //
            tempOldPurse = str1;

            std::string strSender = bPWProtectOldPurse ? oldPurse : nymID;
            std::string strRecipient =
                bPWProtectNewPurse ? newPurse : recipientNymID;

            std::string strSenderAsRecipient =
                nymID;  // Used as the "owner" of newPurseForSender. (So the
                        // sender can recover his sent coins that got encrypted
                        // to someone else's key.);

            // Change the OWNER on token, from NymID to RECIPIENT.
            // (In this block, we change ALL the tokens in the purse.)
            //
            std::string exportedToken = SwigWrap::Token_ChangeOwner(
                notaryID,
                instrumentDefinitionID,
                token,
                nymID,          // signer ID
                strSender,      // old owner
                strRecipient);  // new owner
            // If change failed, then continue.
            //
            if (!VerifyStringVal(exportedToken)) {
                otOut << strLocation
                      << ": 1, OT_API_Token_ChangeOwner "
                         "returned null...(should never "
                         "happen) Returning null.\n";
                return false;
            }

            // SAVE A COPY FOR THE SENDER...
            //
            std::string retainedToken = SwigWrap::Token_ChangeOwner(
                notaryID,
                instrumentDefinitionID,
                token,
                nymID,                  // signer ID
                strSender,              // old owner
                strSenderAsRecipient);  // new owner
            // If change failed, then continue.
            //
            if (!VerifyStringVal(retainedToken)) {
                otOut << strLocation
                      << ":  2, OT_API_Token_ChangeOwner "
                         "returned null...(should never "
                         "happen) Returning null.\n";
                return false;
            }

            //          strSender    = bPWProtectOldPurse ? "" : nymID // unused
            // here. not needed.
            strRecipient = bPWProtectNewPurse ? "" : recipientNymID;

            // PUSH the EXPORTED TOKEN (new owner) into the new purse (again,
            // recipient/newPurse is new owner) and save results in
            // "strPushedForRecipient".
            // Results are, FYI, newPurse+exportedToken.
            //
            std::string strPushedForRecipient = SwigWrap::Purse_Push(
                notaryID,
                instrumentDefinitionID,
                nymID,         // server, asset, signer
                strRecipient,  // owner is either nullptr (for
                               // password-protected
                               // purse) or recipientNymID
                newPurse,
                exportedToken);  // purse, token

            // If push failed, then continue.
            if (!VerifyStringVal(strPushedForRecipient)) {
                otOut << strLocation
                      << ":  OT_API_Purse_Push 1 returned null... "
                         "(should never happen) Returning null.\n";
                return false;
            }

            // PUSH the RETAINED TOKEN (copy for original owner) into the
            // newPurseForSender and save results in "strPushedForRetention".
            // Results are, FYI, newPurseForSender+retainedToken.
            //
            std::string strPushedForRetention = SwigWrap::Purse_Push(
                notaryID,
                instrumentDefinitionID,
                nymID,                 // server, asset, signer
                strSenderAsRecipient,  // This version of the purse is the
                // outgoing copy (for the SENDER's notes).
                // Thus strSenderAsRecipient.
                newPurseForSender,
                retainedToken);  // purse, token

            // If push failed, then continue.
            if (!VerifyStringVal(strPushedForRetention)) {
                otOut << strLocation
                      << ":  OT_API_Purse_Push 2 returned null... "
                         "(should never happen) Returning null.\n";
                return false;
            }

            // Since push succeeded, copy "strPushedForRecipient" (containing
            // newPurse         +exportedToken) into newPurse.
            // Since push succeeded, copy "strPushedForRetention" (containing
            // newPurseForSender+retainedToken) into newPurseForSender.
            //
            newPurse = strPushedForRecipient;
            newPurseForSender = strPushedForRetention;
        }  // for

        // Save tempOldPurse to local storage. (For OLD Owner.)
        // By now, all of the tokens have been popped off of this purse, so it
        // is EMPTY.
        // We're now saving the empty purse, since the user exported all of the
        // tokens.
        //
        // THERE MAYBE SHOULD BE AN EXTRA MODAL HERE, that says,
        // "Moneychanger will now save your purse, EMPTY, back to local storage.
        // Are you sure you want to do this?"
        //

        if (!bPWProtectOldPurse)  // If old purse is NOT password-protected
                                  // (that
                                  // is, it's encrypted to a Nym.)
        {
            if (!SwigWrap::SavePurse(
                    notaryID,
                    instrumentDefinitionID,
                    nymID,
                    tempOldPurse))  // if FAILURE.
            {
                // No error message if saving fails??
                // No modal?
                //
                // FT: adding log.
                otOut << strLocation
                      << ": OT_API_SavePurse "
                         "FAILED. SHOULD NEVER HAPPEN!!!!!!\n";
                return false;
            }
        } else  // old purse IS password protected. (So return its updated
                // version.)
        {
            oldPurse =
                tempOldPurse;  // We never cared about this with Nym-owned
                               // old purse, since it saves to storage
                               // anyway, in the above block. But now in
                               // the case of password-protected purses,
                               // we set the oldPurse to contain the new
                               // version of itself (containing the tokens
                               // that had been left unselected) so the
                               // caller can do what he wills with it.;
        }
    }

    // Else, SPECIFIC TOKENS were selected, so process those only...
    //
    else {
        //      otOut << "Tokens in Cash Purse being processed");

        // newPurseSelectedTokens is created (CORRECTLY) with recipientNymID as
        // owner. (Or with a symmetric key / passphrase.)
        // newPurseUnSelectedTokens is created (CORRECTLY) with NymID as owner.
        // (Unselected tokens aren't being exported...)
        //
        std::string newPurseUnSelectedTokens = SwigWrap::Purse_Empty(
            notaryID,
            instrumentDefinitionID,
            nymID,
            oldPurse);  // Creates an empty copy of oldPurse.;
        std::string newPurseSelectedTokens =
            (bPWProtectNewPurse ? SwigWrap::CreatePurse_Passphrase(
                                      notaryID, instrumentDefinitionID, nymID)
                                : SwigWrap::CreatePurse(
                                      notaryID,
                                      instrumentDefinitionID,
                                      recipientNymID,
                                      nymID));  // recipientNymID = owner,
                                                // nymID = signer;
        std::string newPurseSelectedForSender = SwigWrap::CreatePurse(
            notaryID,
            instrumentDefinitionID,
            nymID,
            nymID);  // nymID = owner, nymID = signer. This is a copy of
                     // newPurseSelectedTokens that's encrypted to the SENDER
                     // (for putting in his outpayments box, so he can still
                     // decrypt if necessary.);

        if (!VerifyStringVal(newPurseSelectedForSender)) {
            otOut << strLocation << ":  OT_API_CreatePurse returned null\n";
            return false;
        }
        if (!VerifyStringVal(newPurseSelectedTokens)) {
            otOut << strLocation
                  << ":  OT_API_CreatePurse or "
                     "OT_API_CreatePurse_Passphrase returned null\n";
            return false;
        }
        if (!VerifyStringVal((newPurseUnSelectedTokens))) {
            otOut << strLocation << ":  OT_API_Purse_Empty returned null\n";
            return false;
        }

        // Iterate through oldPurse, using tempOldPurse as iterator.
        //
        std::int32_t count =
            SwigWrap::Purse_Count(notaryID, instrumentDefinitionID, oldPurse);
        std::string tempOldPurse = oldPurse;

        for (std::int32_t i = 0; i < count; ++i) {
            // Peek at the token on top of the stack.
            // (Without removing it.)
            //
            std::string token = SwigWrap::Purse_Peek(
                notaryID, instrumentDefinitionID, nymID, tempOldPurse);

            // Remove the top token from the stack, and return the updated stack
            // in "str1".
            //
            std::string str1 = SwigWrap::Purse_Pop(
                notaryID, instrumentDefinitionID, nymID, tempOldPurse);

            if (!VerifyStringVal(str1) || !VerifyStringVal(token)) {
                otOut << strLocation
                      << ":  OT_API_Purse_Peek or "
                         "OT_API_Purse_Pop returned null... returning Null. "
                         "(SHOULD NEVER HAPPEN.)\n";
                return false;
            }

            // Putting updated purse into iterator, so any subsequent continues
            // will work properly.
            //
            tempOldPurse = str1;

            // Grab the TokenID for that token. (Token still has OLD OWNER.)
            //
            std::string tokenID =
                SwigWrap::Token_GetID(notaryID, instrumentDefinitionID, token);

            if (!VerifyStringVal(tokenID)) {
                otOut << strLocation
                      << ":  OT_API_Token_GetID returned null... "
                         "SHOULD NEVER HAPPEN. Returning now.\n";
                return false;
            }

            // At this point, we check TokenID (identifying the current token)
            // to see if it's on the SELECTED LIST.
            //
            if (find(selectedTokens.begin(), selectedTokens.end(), tokenID) !=
                selectedTokens.end())  // We ARE exporting
                                       // this token. (Its
                                       // ID was on the
                                       // list.)
            {
                // CHANGE OWNER from NYM to RECIPIENT
                // "token" will now contain the EXPORTED TOKEN, with the NEW
                // OWNER.
                //
                std::string strSender = bPWProtectOldPurse ? oldPurse : nymID;
                std::string strRecipient = bPWProtectNewPurse
                                               ? newPurseSelectedTokens
                                               : recipientNymID;

                std::string strSenderAsRecipient =
                    nymID;  // Used as the "owner" of newPurseSelectedForSender.
                // (So the sender can recover his sent coins that got
                // encrypted to someone else's key.);

                std::string exportedToken = SwigWrap::Token_ChangeOwner(
                    notaryID,
                    instrumentDefinitionID,
                    token,          // server, asset, token,;
                    nymID,          // signer nym
                    strSender,      // old owner
                    strRecipient);  // new owner
                if (!VerifyStringVal(exportedToken)) {
                    otOut << strLocation
                          << ": 1  OT_API_Token_ChangeOwner "
                             "returned null... SHOULD NEVER "
                             "HAPPEN. Returning now.\n";
                    return false;
                }

                std::string retainedToken = SwigWrap::Token_ChangeOwner(
                    notaryID,
                    instrumentDefinitionID,
                    token,                  // server, asset, token,;
                    nymID,                  // signer nym
                    strSender,              // old owner
                    strSenderAsRecipient);  // new owner
                if (!VerifyStringVal(retainedToken)) {
                    otOut << strLocation
                          << ": 2  OT_API_Token_ChangeOwner "
                             "returned null... SHOULD NEVER "
                             "HAPPEN. Returning now.\n";
                    return false;
                }

                // Push exported version of token into new purse for recipient
                // (for selected tokens.)
                //
                //              strSender    = bPWProtectOldPurse ? "" : nymID
                // // unused here. Not needed.
                strRecipient = bPWProtectNewPurse ? "" : recipientNymID;

                std::string strPushedForRecipient = SwigWrap::Purse_Push(
                    notaryID,
                    instrumentDefinitionID,
                    nymID,         // server, asset, signer;
                    strRecipient,  // owner is either nullptr (for
                    // password-protected purse) or recipientNymID
                    newPurseSelectedTokens,
                    exportedToken);  // purse, token
                if (!VerifyStringVal(strPushedForRecipient)) {
                    otOut << strLocation
                          << ":  OT_API_Purse_Push "
                             "newPurseSelectedTokens returned null... "
                             "SHOULD NEVER HAPPEN (returning.)\n";
                    return false;
                }

                // Done: push a copy of these into a purse for the original
                // owner as well, so he has his OWN copy
                // to save in his payments outbox (that HE can decrypt...) so if
                // the cash is lost, for example, he can still
                // recover it. If the recipient receives it and deposits it
                // correctly, the cash in your payment outbox is now
                // worthless and can be discarded, although its existence may be
                // valuable to you as a receipt.
                //
                std::string strPushedForRetention = SwigWrap::Purse_Push(
                    notaryID,
                    instrumentDefinitionID,
                    nymID,  // server, asset, signer;
                    strSenderAsRecipient,
                    newPurseSelectedForSender,
                    retainedToken);  // purse, token
                if (!VerifyStringVal(strPushedForRetention)) {
                    otOut << strLocation
                          << ":  OT_API_Purse_Push "
                             "newPurseSelectedForSender returned null... "
                             "SHOULD NEVER HAPPEN (returning.)\n";
                    return false;
                }

                newPurseSelectedTokens = strPushedForRecipient;
                newPurseSelectedForSender = strPushedForRetention;

            } else  // The token, this iteration, is NOT being exported, but is
                    // remaining with the original owner.
            {
                std::string strSender = bPWProtectOldPurse ? "" : nymID;

                std::string str = SwigWrap::Purse_Push(
                    notaryID,
                    instrumentDefinitionID,
                    nymID,      // server, asset, signer;
                    strSender,  // owner is either nullptr (for
                                // password-protected
                                // purse) or nymID
                    newPurseUnSelectedTokens,
                    token);  // purse, token
                if (!VerifyStringVal(str)) {
                    otOut << strLocation
                          << ": OT_API_Purse_Push "
                             "newPurseUnSelectedTokens returned null... "
                             "SHOULD NEVER HAPPEN. Returning false.\n";
                    return false;
                }

                newPurseUnSelectedTokens = str;
            }
        }  // for

        if (!bPWProtectOldPurse)  // If old purse is NOT password-protected
                                  // (that
                                  // is, it's encrypted to a Nym.)
        {
            if (!SwigWrap::SavePurse(
                    notaryID,
                    instrumentDefinitionID,
                    nymID,
                    newPurseUnSelectedTokens))  // if FAILURE.
            {
                // No error message if saving fails??
                // No modal?
                //
                // FT: adding log.
                otOut << strLocation
                      << ":  OT_API_SavePurse "
                         "FAILED. SHOULD NEVER HAPPEN!!!!!!\n";
                return false;
            }
        } else  // old purse IS password protected. (So return its updated
                // version.)
        {
            oldPurse =
                newPurseUnSelectedTokens;  // We never cared about this with
            // Nym-owned old purse, since it saves
            // to storage anyway, in the above
            // block. But now in the case of
            // password-protected purses, we set
            // the oldPurse to contain the new
            // version of itself (containing the
            // tokens that had been left
            // unselected) so the caller can do
            // what he wills with it.;
        }

        // The SELECTED tokens (with Recipient as owner of purse AND tokens
        // within) are returned as the "newPurse".
        // The SELECTED tokens (with Sender as owner of purse AND tokens within)
        // are returned as "newPurseForSender".
        //
        newPurse = newPurseSelectedTokens;
        newPurseForSender = newPurseSelectedForSender;
    }

    return true;
}
#endif  // OT_CASH

int32_t CmdBaseInstrument::sendPayment(
    const string& cheque,
    string sender,
    const char* what) const
{
    string server = SwigWrap::Instrmnt_GetNotaryID(cheque);
    if ("" == server) {
        otOut << "Error: cannot get server.\n";
        return -1;
    }

    if ("" == sender) {
        sender = SwigWrap::Instrmnt_GetSenderNymID(cheque);
        if ("" == sender) {
            otOut << "Error: cannot get sender.\n";
            return -1;
        }
    }

    string recipient = SwigWrap::Instrmnt_GetRecipientNymID(cheque);
    if ("" == recipient) {
        otOut << "Error: cannot get recipient.\n";
        return -1;
    }

    std::unique_ptr<OTPayment> payment =
        std::make_unique<OTPayment>(String(cheque.c_str()));
    string response = OT::App()
                          .API()
                          .ServerAction()
                          .SendPayment(
                              Identifier(sender),
                              Identifier(server),
                              Identifier(recipient),
                              payment)
                          ->Run();
    return processResponse(response, what);
}

string CmdBaseInstrument::writeCheque(
    string myacct,
    string hisnym,
    string amount,
    string memo,
    string validfor,
    bool isInvoice) const
{
    if (!checkAccount("myacct", myacct)) {
        return "";
    }

    if ("" != hisnym && !checkNym("hisnym", hisnym)) {
        return "";
    }

    int64_t value = checkAmount("amount", amount, myacct);
    if (OT_ERROR_AMOUNT == value) {
        return "";
    }

    if ("" != validfor && !checkValue("validfor", validfor)) {
        return "";
    }

    string server = SwigWrap::GetAccountWallet_NotaryID(myacct);
    if ("" == server) {
        otOut << "Error: cannot determine server from myacct.\n";
        return "";
    }

    string mynym = SwigWrap::GetAccountWallet_NymID(myacct);
    if ("" == mynym) {
        otOut << "Error: cannot determine mynym from myacct.\n";
        return "";
    }

    if (!OT::App().API().ServerAction().GetTransactionNumbers(
            Identifier(mynym), Identifier(server), 10)) {
        otOut << "Error: cannot reserve transaction numbers.\n";
        return "";
    }

    int64_t oneMonth = OTTimeGetSecondsFromTime(OT_TIME_MONTH_IN_SECONDS);
    int64_t timeSpan = "" != validfor ? stoll(validfor) : oneMonth;
    time64_t from = SwigWrap::GetTime();
    time64_t until = OTTimeAddTimeInterval(from, timeSpan);

    string cheque = SwigWrap::WriteCheque(
        server,
        isInvoice ? -value : value,
        from,
        until,
        myacct,
        mynym,
        memo,
        hisnym);
    if ("" == cheque) {
        otOut << "Error: cannot write cheque.\n";
        return "";
    }

    // Record it in the records?
    // Update: We wouldn't record that here. Instead, SwigWrap::WriteCheque
    // should drop a notice into the payments outbox, the same as it does when
    // you "sendcheque" (after all, the same resolution would be expected once
    // it is cashed.)

    return cheque;
}
