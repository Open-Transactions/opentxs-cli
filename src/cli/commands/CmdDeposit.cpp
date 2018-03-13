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

#include "CmdDeposit.hpp"

#include "CmdBase.hpp"

#include <opentxs/api/client/ServerAction.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/api/Api.hpp>
#include <opentxs/cash/Purse.hpp>
#include <opentxs/client/ServerAction.hpp>
#include <opentxs/client/SwigWrap.hpp>
#include <opentxs/client/Utility.hpp>
#include <opentxs/core/Log.hpp>
#include <opentxs/core/Cheque.hpp>
#include <opentxs/OT.hpp>

#include <stdint.h>
#include <ostream>
#include <string>
#include <vector>

using namespace opentxs;
using namespace std;

CmdDeposit::CmdDeposit()
{
    command = "deposit";
    args[0] = "--myacct <account>";
    args[1] = "[--mynym <nym>]";
    args[2] = "[--indices <indices|all>]";
    category = catAccounts;
    help = "Deposit cash, cheque, voucher, or invoice.";
    usage =
        "Any supplied indices must correspond to tokens in your cash purse.";
}

CmdDeposit::~CmdDeposit() {}

int32_t CmdDeposit::runWithOptions()
{
    return run(getOption("mynym"), getOption("myacct"), getOption("indices"));
}

// A bit complicated:
//
// If I specify MyPurse and MyAcct, then they MUST have the same instrument
// definition.
// If I specify MyNym and MyPurse, that is where we will look for the purse.
// If I specify MyAcct, and it's owned by a different Nym than MyNym, then
// the cash tokens will be reassigned from MyNym to MyAcct's Nym, before
// depositing.
// Basically ALWAYS look up MyAcct's owner, and set HIM as the recipient Nym.
// (But still use MyNym, independently, to find the purse being deposited.)
//
// Must ALWAYS specify MyAcct because otherwise, where are you depositing to?
//
// If MyNym isn't available, should use MyAcct's Nym.
//
// Shouldn't need to specify MyPurse, since we can ONLY deposit into MyAcct of
// the same type as MyAcct. Thus we should ignore any other instrument
// definitions or
// purses since they couldn't possibly be deposited into MyAcct anyway.

int32_t CmdDeposit::run(string mynym, string myacct, string indices)
{
    if (!checkAccount("myacct", myacct)) {
        return -1;
    }

    string server = SwigWrap::GetAccountWallet_NotaryID(myacct);
    if ("" == server) {
        otOut << "Error: cannot determine server from myacct.\n";
        return -1;
    }

    string toNym = SwigWrap::GetAccountWallet_NymID(myacct);
    if ("" == toNym) {
        otOut << "Error: cannot determine toNym from myacct.\n";
        return -1;
    }

    if ("" != indices) {
        // Only in the case of cash, it's possible you have some cash in Nym A's
        // purse, but you want to deposit it into Nym B's account. So we have a
        // "to" Nym and a "from" Nym even though they will often be the same.
        string fromNym = toNym;
        if ("" != mynym) {
            if (!checkNym("mynym", mynym)) {
                return -1;
            }
            fromNym = mynym;
        }

        // In this case, instrument is blank. That's how the callee knows that
        // we're working with the local purse. Then indices tells him either to
        // use "all" tokens in that purse, or the selected indices.
        return depositPurse(server, myacct, fromNym, "", indices);
    }

    otOut << "You can deposit a PURSE (containing cash tokens) or "
             "a CHEQUE / INVOICE / VOUCHER.\n";

    string instrument = inputText("your financial instrument");
    if ("" == instrument) {
        return -1;
    }

    string type = SwigWrap::Instrmnt_GetType(instrument);
    if ("PURSE" == type) {
        return depositPurse(server, myacct, toNym, instrument, "");
    }

    if ("CHEQUE" == type || "INVOICE" == type || "VOUCHER" == type) {
        return depositCheque(server, myacct, toNym, instrument);
    }

    otOut << "Error: cannot determine instrument type.\n"
             "Expected CHEQUE, VOUCHER, INVOICE, or (cash) PURSE.\n";
    return -1;
}

#if OT_CASH
std::int32_t CmdDeposit::depositCashPurse(
    const std::string& notaryID,
    const std::string& instrumentDefinitionID,
    const std::string& nymID,
    const std::string& oldPurse,
    const std::vector<std::string>& selectedTokens,
    const std::string& accountID,
    bool bReimportIfFailure,  // So we don't re-import a purse that wasn't
                              // internal to begin with.
    std::string* pOptionalOutput /*=nullptr*/) const
{
    std::string recipientNymID = SwigWrap::GetAccountWallet_NymID(accountID);
    if (!VerifyStringVal(recipientNymID)) {
        otOut << "\ndepositCashPurse: Unable to find recipient Nym based on "
                 "myacct. \n";
        return -1;
    }

    bool bPasswordProtected = SwigWrap::Purse_HasPassword(notaryID, oldPurse);

    std::string newPurse;                // being deposited.;
    std::string newPurseForSender = "";  // Probably unused in this case.;
    std::string copyOfOldPurse = oldPurse;
    bool bSuccessProcess = processCashPurse(
        newPurse,
        newPurseForSender,
        notaryID,
        instrumentDefinitionID,
        nymID,
        copyOfOldPurse,
        selectedTokens,
        recipientNymID,
        bPasswordProtected,
        false);

    if (!bSuccessProcess || !VerifyStringVal(newPurse)) {
        otOut << "OT_ME_depositCashPurse: new Purse is empty, after processing "
                 "it for deposit. \n";
        return -1;
    }

    std::unique_ptr<Purse> purse(Purse::PurseFactory(String(newPurse)));

    OT_ASSERT(purse);

    auto action = OT::App().API().ServerAction().DepositCashPurse(
        Identifier(nymID), Identifier(notaryID), Identifier(accountID), purse);
    std::string strResponse = action->Run();
    std::string strAttempt = "deposit_cash";

    // HERE, WE INTERPRET THE SERVER REPLY, WHETHER SUCCESS, FAIL, OR ERROR...

    std::int32_t nInterpretReply = InterpretTransactionMsgReply(
        notaryID, recipientNymID, accountID, strAttempt, strResponse);

    if (1 == nInterpretReply) {

        if (nullptr != pOptionalOutput) *pOptionalOutput = strResponse;

        // Download all the intermediary files (account balance, inbox, outbox,
        // etc)
        // since they have probably changed from this operation.
        //
        bool bRetrieved = OT::App().API().ServerAction().DownloadAccount(
            Identifier(recipientNymID),
            Identifier(notaryID),
            Identifier(accountID),
            true);  // bForceDownload defaults to false.;

        otOut << "\nServer response (" << strAttempt
              << "): SUCCESS depositing cash!\n";
        otOut << std::string(bRetrieved ? "Success" : "Failed")
              << " retrieving intermediary files for account.\n";
    } else  // failure. (so we re-import the cash, so as not to lose it...)
    {

        if (!bPasswordProtected && bReimportIfFailure) {
            bool importStatus = SwigWrap::Wallet_ImportPurse(
                notaryID, instrumentDefinitionID, recipientNymID, newPurse);
            otOut << "Since failure in OT_ME_depositCashPurse, "
                     "OT_API_Wallet_ImportPurse called. Status of "
                     "import: "
                  << importStatus << "\n";

            if (!importStatus) {
                // Raise the alarm here that we failed depositing the purse, and
                // then we failed
                // importing it back into our wallet again.
                otOut << "Error: Failed depositing the cash purse, and then "
                         "failed re-importing it back to wallet. Therefore YOU "
                         "must copy the purse NOW and save it to a safe place! "
                         "\n";

                otOut << newPurse << "\n";

                otOut << "AGAIN: Be sure to copy the above purse "
                         "to a safe place, since it FAILED to "
                         "deposit and FAILED to re-import back "
                         "into the wallet. \n";
            }
        } else {
            otOut << "Error: Failed depositing the cash purse. "
                     "Therefore YOU must copy the purse NOW and "
                     "save it to a safe place! \n";

            otOut << newPurse << "\n";

            otOut << "AGAIN: Be sure to copy the above purse to a "
                     "safe place, since it FAILED to deposit. \n";
        }

        return -1;
    }

    //
    // Return status to caller.
    //
    return nInterpretReply;
}
#endif  // OT_CASH

// THESE FUNCTIONS were added for the PAYMENTS screen. (They are fairly new.)
//
// Basically there was a need to have DIFFERENT instruments, but to be able to
// treat them as though they are a single type.
//
// In keeping with that, the below functions will work with disparate types.
// You can pass [ CHEQUES / VOUCHERS / INVOICES ] and PAYMENT PLANS, and
// SMART CONTRACTS, and PURSEs into these functions, and they should be able
// to handle any of those types.

int32_t CmdDeposit::depositCheque(
    const string& server,
    const string& myacct,
    const string& mynym,
    const string& instrument,
    string* pOptionalOutput /*=nullptr*/) const
{
    string assetType = getAccountAssetType(myacct);
    if ("" == assetType) {
        return -1;
    }

    if (assetType != SwigWrap::Instrmnt_GetInstrumentDefinitionID(instrument)) {
        otOut << "Error: instrument definitions of instrument and myacct do "
                 "not match.\n";
        return -1;
    }

    std::unique_ptr<Cheque> cheque = std::make_unique<Cheque>();
    cheque->LoadContractFromString(String(instrument.c_str()));

    string response = OT::App()
                          .API()
                          .ServerAction()
                          .DepositCheque(
                              Identifier(mynym),
                              Identifier(server),
                              Identifier(myacct),
                              cheque)
                          ->Run();
    int32_t reply =
        responseReply(response, server, mynym, myacct, "deposit_cheque");
    if (1 != reply) {
        return reply;
    }

    if (nullptr != pOptionalOutput) *pOptionalOutput = response;

    if (!OT::App().API().ServerAction().DownloadAccount(
            Identifier(mynym), Identifier(server), Identifier(myacct), true)) {
        otOut << "Error retrieving intermediary files for account.\n";
        return -1;
    }

    return 1;
}

int32_t CmdDeposit::depositPurse(
    const string& server,
    const string& myacct,
    const string& mynym,
    string instrument,
    const string& indices,
    string* pOptionalOutput /*=nullptr*/) const
{
#if OT_CASH
    string assetType = getAccountAssetType(myacct);
    if ("" == assetType) {
        return -1;
    }

    if ("" != instrument) {
        vector<string> tokens;
        return depositCashPurse(
            server,
            assetType,
            mynym,
            instrument,
            tokens,
            myacct,
            false,
            pOptionalOutput);
    }

    // we have to load the purse ourselves
    instrument = SwigWrap::LoadPurse(server, assetType, mynym);
    if ("" == instrument) {
        otOut << "Error: cannot load purse.\n";
        return -1;
    }

    vector<string> tokens;
    if (0 > getTokens(tokens, server, mynym, assetType, instrument, indices)) {
        return -1;
    }

    return depositCashPurse(
        server,
        assetType,
        mynym,
        instrument,
        tokens,
        myacct,
        true,
        pOptionalOutput);
#else
    return -1;
#endif  // OT_CASH
}
