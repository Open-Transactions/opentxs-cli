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

#include "CmdConfirm.hpp"

#include "CmdShowNyms.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <map>
#include <string>
#include <utility>

using namespace opentxs;
using namespace std;

CmdConfirm::CmdConfirm()
{
    command = "confirm";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "[--myacct <account>]";
    args[3] = "[--hisnym <nym>]";
    args[4] = "[--index <index>]";
    category = catInstruments;
    help = "Confirm your agreement to a smart contract or payment plan.";
}

CmdConfirm::~CmdConfirm() {}

int32_t CmdConfirm::runWithOptions()
{
    return run(
        getOption("server"),
        getOption("mynym"),
        getOption("myacct"),
        getOption("hisnym"),
        getOption("index"));
}

int32_t CmdConfirm::run(
    string server,
    string mynym,
    string myacct,
    string hisnym,
    string index)
{
    if (!checkServer("server", server)) {
        return -1;
    }

    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    if ("" != myacct && !checkAccount("myacct", myacct)) {
        return -1;
    }

    if ("" == index) {
        string instrument = inputText("a smart contract or payment plan");
        if ("" == instrument) {
            return -1;
        }

        return confirmInstrument(server, mynym, myacct, hisnym, instrument, -1);
    }

    string inbox = SwigWrap::LoadPaymentInbox(server, mynym);
    if ("" == inbox) {
        otOut << "Error: cannot load payment inbox.\n";
        return -1;
    }

    int32_t items = SwigWrap::Ledger_GetCount(server, mynym, mynym, inbox);
    if (0 > items) {
        otOut << "Error: cannot load payment inbox item count.\n";
        return -1;
    }

    if (0 == items) {
        otOut << "The payment inbox is empty.\n";
        return 0;
    }

    int32_t messageNr = checkIndex("index", index, items);
    if (0 > messageNr) {
        return -1;
    }

    // use specified payment instrument from inpayments
    string instrument =
        OTRecordList::get_payment_instrument(server, mynym, messageNr, "");
    if (instrument.empty()) {
        otOut << "Error: cannot load payment instrument.\n";
        return -1;
    }

    return confirmInstrument(
        server, mynym, myacct, hisnym, instrument, messageNr);
}

int32_t CmdConfirm::confirmInstrument(
    const string& server,
    const string& mynym,
    const string& myacct,
    const string& hisnym,
    const string& instrument,
    int32_t index,
    std::string* pOptionalOutput /*=nullptr*/)
{
    string instrumentType = SwigWrap::Instrmnt_GetType(instrument);
    if (instrumentType.empty()) {
        otOut << "Error: instrument is empty.\n";
        return -1;
    }

    time64_t now = SwigWrap::GetTime();
    time64_t from = SwigWrap::Instrmnt_GetValidFrom(instrument);
    if (now < from) {
        otOut << "The instrument is not yet valid.\n";
        return 0;
    }

    time64_t until = SwigWrap::Instrmnt_GetValidTo(instrument);
    if (until > OT_TIME_ZERO && now > until) {
        otOut << "The instrument has expired.\n";

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

    if ("PAYMENT PLAN" == instrumentType) {
        return confirmPaymentPlan(mynym, myacct, instrument, pOptionalOutput);
    }

    if ("SMARTCONTRACT" == instrumentType) {
        return confirmSmartContract(
            server, mynym, myacct, hisnym, instrument, index, pOptionalOutput);
    }

    // CHEQUE VOUCHER INVOICE PURSE
    otOut << "Error: instrument is not a smart contract or payment plan.\n";
    return -1;
}

int32_t CmdConfirm::confirmPaymentPlan(
    const std::string& mynym,
    const std::string& myacct,
    const std::string& plan,
    std::string* pOptionalOutput /*=nullptr*/)
{
    return OTRecordList::confirmPaymentPlan_lowLevel(
        mynym, myacct, plan, pOptionalOutput);
}

// NOTE: if index is -1, then it's assumed the instrument was PASTED in, and
// therefore the notaryID must be found on the instrument, or must be provided
// as Server, or we must ask the user to enter it. Then the Nym [and potentially
// AcctID] must be ascertained by walking the user through the instrument, and
// by making him choose a Nym and Acct(s) for the party he's confirming as.
//
// But if an index is provided, then it's ASSUMED the index goes to the payments
// inbox, and that therefore Server and MyNym must have been provided at the
// command line already, or otherwise that index could not logically have been
// matched up to the proper box.
//
// If SOME parties have already confirmed the contract before you, then it
// SHOULD already have a notaryID attached to it. If a Server is also provided
// at the command line, then the two must match, since the ID cannot be changed
// after that point.

int32_t CmdConfirm::confirmSmartContract(
    const string& server,
    const string& mynym,
    const string& myacct,
    const string& hisnym,
    const string& contract,
    int32_t index,
    string* pOptionalOutput /*=nullptr*/)
{
    int32_t parties = SwigWrap::Smart_GetPartyCount(contract);
    if (0 > parties) {
        otOut << "Error: cannot get contract party count.\n";
        return -1;
    }

    if (0 == parties) {
        otOut << "Error: there are no contract parties.\n";
        return -1;
    }

    if (SwigWrap::Smart_AreAllPartiesConfirmed(contract)) {
        otOut << "Error: all contract parties are already confirmed.\n";
        return -1;
    }

    int32_t unconfirmed = 0;
    for (int32_t i = 0; i < parties; i++) {
        string name = SwigWrap::Smart_GetPartyByIndex(contract, i);
        if ("" == name) {
            otOut << "Error: cannot retrieve contract party name.\n";
            return -1;
        }

        if (!SwigWrap::Smart_IsPartyConfirmed(contract, name)) {
            otOut << i << ": Unconfirmed party: " << name << "\n";
            unconfirmed++;
        }
    }

    if (0 == unconfirmed) {
        otOut << "Error: cannot find unconfirmed contract parties.\n";
        return -1;
    }

    cout << "\nWhich party are you? Enter the number, from the list above: ";
    int32_t party = checkIndex("party number", inputLine(), parties);
    if (0 > party) {
        return -1;
    }

    string name = SwigWrap::Smart_GetPartyByIndex(contract, party);
    if ("" == name) {
        otOut << "Error: cannot retrieve contract party name.\n";
        return -1;
    }

    if (SwigWrap::Smart_IsPartyConfirmed(contract, name)) {
        otOut << "Error: contract party was already confirmed.\n";
        return -1;
    }

    // So how many accounts does the party have? We must confirm those, too.
    int32_t accounts = SwigWrap::Party_GetAcctCount(contract, name);
    if (0 > accounts) {
        otOut << "Error: cannot load party account item count.\n";
        return -1;
    }

    if (0 < accounts) {
        if (!confirmAccounts(server, mynym, myacct, contract, name, accounts)) {
            return -1;
        }
    }

    // CONFIRM THE NYM HERE.
    //
    // NOTE: confirming the Party will drop a copy into the outpayments box.
    // After that, if you are not the last party, you will send it on to the
    // next party. This will also drop it into the outpayments box, but when
    // that happens, it will automatically first remove the prior one that had
    // been dropped. This way there is only one copy in the outbox, not two.
    //
    // If you ARE the last party, then we will activate it here, and when the
    // server reply is received, it will be removed from the outbox and moved
    // to the record box.
    //
    // Also, whether it succeeds or fails activation, either way, a notice will
    // be dropped to all the parties, informing them of this fact. The
    // activating party will ignore this, since he already processes the server
    // reply directly when he receives it. And there's a copy of that in his
    // nymbox to make SURE he receives it, so he DEFINITELY received it already.
    // But each of the other parties will then move the notice from their
    // outbox to their record box. (And harvest the transaction numbers
    // accordingly.)
    // A party can "reject" a smart contract by activating it without signing
    // it. That way it will fail activation, and all the other parties will get
    // the failure notice, and harvest their numbers back as appropriate. But if
    // he is "rude" and simply discards the contract WITHOUT rejecting it, then
    // they will never get the failure notice. However, they will also never get
    // the activation notice, since it was never activated in that case, and
    // thus they will be safe to harvest their numbers back when it expires.
    // (A well-designed wallet will do this automatically.)

    string confirmed =
        SwigWrap::SmartContract_ConfirmParty(contract, name, mynym, server);
    if ("" == confirmed) {
        otOut << "Error: cannot confirm smart contract party.\n";
        return harvestTxNumbers(contract, mynym);
    }

    if (nullptr != pOptionalOutput) *pOptionalOutput = confirmed;

    if (SwigWrap::Smart_AreAllPartiesConfirmed(confirmed)) {
        // If you are the last party to sign, then ACTIVATE THE SMART CONTRACT.
        return activateContract(server, mynym, confirmed, name, accounts);
    }

    // NOTE: No matter which party you are (perhaps you are the middle one),
    // when you confirm the contract, you will send it on to the NEXT
    // UNCONFIRMED ONE. This means you don't know which party it will be,
    // since all the unconfirmed parties have no NymID (yet.) Rather, it's
    // YOUR problem to provide the NymID you're sending the contract on to.
    // And then it's HIS problem to decide which party he will sign on as.
    // (Unless you are the LAST PARTY to confirm, in which case YOU are the
    // activator.)
    int32_t success = sendToNextParty(server, mynym, hisnym, confirmed);
    if (1 != success) {
        return success;
    }

    if (-1 != index) {
        // not a pasted contract
        SwigWrap::RecordPayment(server, mynym, true, index, false);
    }

    return 1;
}

int32_t CmdConfirm::activateContract(
    const string& server,
    const string& mynym,
    const string& contract,
    const string& name,
    int32_t accounts)
{
    // We don't need MyAcct except when actually ACTIVATING the smart contract
    // on the server. This variable might get set later to MyAcct, if it matches
    // one of the accounts being confirmed. (Meaning if this variable is set by
    // the time we reach the bottom, then we can use it for activation, if/when
    // needed.)
    string myAcctID = "";
    string myAcctAgentName = "";

    // We need the ACCT_ID that we're using to activate it with, and we need the
    // AGENT NAME for that account.
    if ("" == myAcctID || "" == myAcctAgentName) {
        // We can't just use ANY account ID, but we must use one that is listed
        // as one of the accounts for the party activating the contract. So we
        // have to display those accounts, and the user must choose which one
        // it's going to be. From there we can get the account ID and the agent
        // name and call activate_smart_contract.
        showPartyAccounts(contract, name, 2);

        otOut << "\nEnter the index for the account you'll use to ACTIVATE the "
                 "smart contract: ";

        int32_t acctIndex = checkIndex("account index", inputLine(), accounts);
        if (0 > acctIndex) {
            return harvestTxNumbers(contract, mynym);
        }

        string acctName =
            SwigWrap::Party_GetAcctNameByIndex(contract, name, acctIndex);
        if ("" == acctName) {
            otOut << "Error: cannot retrieve account.\n";
            return harvestTxNumbers(contract, mynym);
        }

        myAcctID = SwigWrap::Party_GetAcctID(contract, name, acctName);
        if ("" == myAcctID) {
            otOut << "Error: account is not yet confirmed.\n";
            return harvestTxNumbers(contract, mynym);
        }

        myAcctAgentName =
            SwigWrap::Party_GetAcctAgentName(contract, name, acctName);
        if ("" == myAcctAgentName) {
            otOut << "Error: account agent is not yet confirmed.\n";
            return harvestTxNumbers(contract, mynym);
        }
    }

    const Identifier theNotaryID{server}, theNymID{mynym}, theAcctID{myAcctID};

    std::unique_ptr<OTSmartContract> smartContract =
        std::make_unique<OTSmartContract>();

    OT_ASSERT(smartContract)

    smartContract->LoadContractFromString(String(contract));

    std::string response;
    {
        response = OT::App()
                          .API()
                          .ServerAction()
                          .ActivateSmartContract(
                              theNymID,
                              theNotaryID,
                              theAcctID,
                              myAcctAgentName,
                              smartContract)
                          ->Run();
    }
    if (1 != responseStatus(response)) {
        otOut << "Error: cannot activate smart contract.\n";
        harvestTxNumbers(contract, mynym);
        return -1;
    }

    // BELOW THIS POINT, the transaction has definitely processed.
    int32_t reply = responseReply(
        response, server, mynym, myAcctID, "activate_smart_contract");
    if (1 != reply) {
        return reply;
    }

    {
        if (!OT::App().API().ServerAction().DownloadAccount(
                theNymID, theNotaryID, theAcctID, true)) {
            otOut << "Error retrieving intermediary files for account.\n";
        }
    }

    return 1;
}

int32_t CmdConfirm::sendToNextParty(
    const string& server,
    const string& mynym,
    const string& hisnym,
    const string& contract)
{
    // But if all the parties are NOT confirmed, then we need to send it to
    // the next guy. In that case:
    // If HisNym is provided, and it's different than mynym, then use it.
    // He's the next receipient.
    // If HisNym is NOT provided, then display the list of NymIDs, and allow
    // the user to paste one. We can probably select him based on abbreviated
    // ID or Name as well (I think there's an API call for that...)
    string hisNymID = hisnym;

    // If hisNymID doesn't exist, or it's the same as mynym, then ask
    // the user to select a NymID for the recipient.
    if ("" == hisNymID || hisNymID == mynym) {
        CmdShowNyms showNyms;
        showNyms.run();

        otOut << "\nOnce you confirm this contract, then we need to send it on "
                 "to the\nnext party so he can confirm it, too.\n\nPlease "
                 "PASTE a recipient Nym ID (the next party): ";
        string recipientNymID = inputLine();
        if ("" == recipientNymID) {
            otOut << "Error: no recipient nym found.\n";
            return harvestTxNumbers(contract, mynym);
        }

        // IF we are able to resolve the HisNymId from a partial, then we
        // replace the partial with the full version. (Otherwise we assume
        // it's already a full ID and we don't mess with it.)
        hisNymID = SwigWrap::Wallet_GetNymIDFromPartial(recipientNymID);
        if ("" == hisNymID) {
            hisNymID = recipientNymID;
        }

        if (hisNymID == mynym) {
            otOut << "\nSorry, but YOU cannot simultaneously be the SENDER "
                     "_and_ the RECIPIENT. Choose another nym for one or the "
                     "other.\n";
            return harvestTxNumbers(contract, mynym);
        }
    }

    auto payment = std::make_shared<const OTPayment>(String(contract.c_str()));
    std::string response;
    {
        response = OT::App()
                          .API()
                          .ServerAction()
                          .SendPayment(
                              Identifier(mynym),
                              Identifier(server),
                              Identifier(hisNymID),
                              payment)
                          ->Run();
        if (1 != responseStatus(response)) {
            otOut << "\nFor whatever reason, our attempt to send the instrument on "
            "to the next user has failed.\n";
            return harvestTxNumbers(contract, mynym);
        }
    }

    // Success. (Remove the payment instrument we just successfully sent from
    // our payments inbox.)

    // In the case of smart contracts, it might be sent on to a chain of 2 or
    // 3 users, before finally being activated by the last one in the chain.
    // All of the users in the chain (except the first one) will thus have a
    // copy of the smart contract in their payments inbox AND outbox.
    //
    // But once the smart contract has successfully been sent on to the next
    // user, and thus a copy of it is in my outbox already, then there is
    // definitely no reason for a copy of it to stay in my inbox as well.
    // Might as well remove that copy.
    //
    // We can't really expect to remove the payments inbox copy inside OT
    // itself, when we receive the server's SendNymInstrumentResponse reply
    // message,
    // without opening up the (encrypted) contents. (Although that would
    // actually be ideal, since it would cover all cases included dropped
    // messages...) But we CAN easily remove it RIGHT HERE.
    // Perhaps in the future I WILL move this code to the
    // SendNymInstrumentResponse
    // reply processing, but that will require it to be encrypted to my own
    // key as well as the recipient's, which we already do for sending cash,
    // but which we up until now have not done for the other instruments.
    // So perhaps we'll start doing that sometime in the future, and then move
    // this code.
    //
    // In the meantime, this is good enough.

    otOut << "Success sending the agreement on to the next party.\n";
    return 1;
}

int32_t CmdConfirm::confirmAccounts(
    string server,
    string mynym,
    string myacct,
    string contract,
    const string& name,
    int32_t accounts)
{
    map<string, string> mapIDs;
    map<string, string> mapAgents;

    // In the loop, if we have to devise the notaryID, we store
    // it in this var so we don't devise twice.
    string foundNotaryID = "";
    string foundMyNymID = "";

    while (0 < accounts) {
        otOut << "\n";
        showPartyAccounts(contract, name, 2);

        otOut << "\nThere are " << accounts
              << " asset accounts remaining to "
                 "be confirmed.\nEnter the index "
                 "for an UNconfirmed account: ";

        int32_t acctIndex = checkIndex("account index", inputLine(), accounts);
        if (0 > acctIndex) {
            return -1;
        }

        string acctName =
            SwigWrap::Party_GetAcctNameByIndex(contract, name, acctIndex);
        if ("" == acctName) {
            otOut << "Error: cannot retrieve unconfirmed account.\n";
            return -1;
        }

        bool alreadyThere = false;
        for (auto x = mapIDs.begin(); x != mapIDs.end(); x++) {
            if (x->first == acctName) {
                alreadyThere = true;
                break;
            }
        }

        string acctID = SwigWrap::Party_GetAcctID(contract, name, acctName);
        if (alreadyThere || "" != acctID) {
            otOut << "The account at index " << acctIndex
                  << " is already confirmed with account ID: " << acctID
                  << "\nPlease pick a different account.\n";
            continue;
        }

        // The account is NOT already confirmed (so we can confirm it,
        // once we select the Acct ID to use.)

        string serverFromContract = SwigWrap::Instrmnt_GetNotaryID(contract);
        if ("" != serverFromContract && server != serverFromContract) {
            otOut << "Error: mismatching server in contract.\n";
            return -1;
        }

        if (!SwigWrap::IsNym_RegisteredAtServer(mynym, server)) {
            otOut << "Error: mynym is not registered on server.\n";
            return -1;
        }

        // Loop through all the accounts for that Server/Nym, display them,
        // allow the user to choose one and then store it in a list somewhere
        // for the actual confirmation. (Which we will do only after ALL
        // accounts have been selected.) We will also make sure to prevent
        // from selecting an account that already appears on that list.
        //
        // Also, when we actually add an Acct ID to the list, we need to
        // decrement accounts, so that we are finished once all the accounts
        // from the smart contract template have had actual acct IDs selected
        // for each.
        string templateInstrumentDefinitionID =
            SwigWrap::Party_GetAcctInstrumentDefinitionID(
                contract, name, acctName);
        bool foundTemplateInstrumentDefinitionID =
            "" != templateInstrumentDefinitionID;

        bool foundAccounts = false;
        int32_t accountCount = SwigWrap::GetAccountCount();

        otOut << "\nAccounts by index (filtered by notaryID and nymID):\n\n";

        for (int32_t i = 0; i < accountCount; i++) {
            string acct = SwigWrap::GetAccountWallet_ID(i);
            if ("" == acct) {
                otOut << "Error reading account ID based on index: " << i
                      << "\n";
                return -1;
            }

            string acctNotaryID = SwigWrap::GetAccountWallet_NotaryID(acct);
            string acctNymID = SwigWrap::GetAccountWallet_NymID(acct);
            string acctInstrumentDefinitionID =
                SwigWrap::GetAccountWallet_InstrumentDefinitionID(acct);

            bool bAlreadyOnTheMap = false;
            for (auto x = mapIDs.begin(); x != mapIDs.end(); x++) {
                if (x->second == acct) {
                    bAlreadyOnTheMap = true;
                    break;
                }
            }

            if (server == acctNotaryID && mynym == acctNymID) {
                // If the smart contract doesn't specify the instrument
                // definition ID of
                // the account, or if it DOES specify, AND they match, then
                // it's a viable choice. Display it.
                if (!foundTemplateInstrumentDefinitionID ||
                    templateInstrumentDefinitionID ==
                        acctInstrumentDefinitionID) {
                    // DO NOT display any accounts that have already been
                    // selected! (Search the temp map where we've been stashing
                    // them.)
                    if (!bAlreadyOnTheMap) {
                        foundAccounts = true;
                        otOut << i << " : " << acct << " ("
                              << SwigWrap::GetAccountWallet_Name(acct) << ")\n";
                    }
                }
            }
        }

        if (!foundAccounts) {
            otOut << "There are no accounts matching the specified Nym ("
                  << mynym << ") and Server (" << server
                  << ")\nTry:  opentxs newaccount --mynym " << mynym
                  << " --server " << server << " \n";
            return -1;
        }

        otOut << "\nChoose an account by index (for '" << acctName << "'): ";

        string selectedAcctIndex = inputLine();
        if ("" == selectedAcctIndex) {
            return -1;
        }

        int32_t selectedIndex = stol(selectedAcctIndex);
        if (0 > selectedIndex || selectedIndex >= accountCount) {
            otOut << "Bad index: " << selectedAcctIndex << "\n";
            return -1;
        }

        string acct = SwigWrap::GetAccountWallet_ID(selectedIndex);
        if ("" == acct) {
            otOut << "Error reading account ID based on index: "
                  << selectedIndex << "\n";
            return -1;
        }

        string acctNotaryID = SwigWrap::GetAccountWallet_NotaryID(acct);
        string acctNymID = SwigWrap::GetAccountWallet_NymID(acct);
        string acctInstrumentDefinitionID =
            SwigWrap::GetAccountWallet_InstrumentDefinitionID(acct);

        if (server == acctNotaryID && mynym == acctNymID) {
            // If the smart contract doesn't specify the instrument definition
            // ID of the
            // account, or if it DOES specify, AND they match, then it's a
            // viable choice. Allow it.
            if (!foundTemplateInstrumentDefinitionID ||
                templateInstrumentDefinitionID == acctInstrumentDefinitionID) {

                bool bAlreadyOnIt = false;
                for (auto x = mapIDs.begin(); x != mapIDs.end(); x++) {
                    if (x->second == acct) {
                        bAlreadyOnIt = true;
                        break;
                    }
                }

                if (bAlreadyOnIt) {
                    otOut << "Sorry, you already selected this account. Choose "
                             "another.\n";
                } else {
                    // acct has been selected for name's account, acctName.
                    // Add these to a map or whatever, to save them until
                    // this loop is complete.

                    string agentName = SwigWrap::Party_GetAcctAgentName(
                        contract, name, acctName);

                    if ("" == agentName) {
                        otOut << "\n";

                        if (showPartyAgents(contract, name, 3)) {
                            otOut << "\n (This choice is arbitrary, but you "
                                     "must pick one.)\nEnter the index for an "
                                     "agent, to have authority  over that "
                                     "account: ";

                            string strAgentIndex = inputLine();
                            if ("" == strAgentIndex) {
                                return -1;
                            }

                            int32_t nAgentIndex = stol(strAgentIndex);
                            if (0 > nAgentIndex) {
                                otOut << "Error: Bad Index: " << strAgentIndex
                                      << "\n";
                                return -1;
                            }

                            agentName = SwigWrap::Party_GetAgentNameByIndex(
                                contract, name, nAgentIndex);
                            if ("" == agentName) {
                                otOut << "Error: Unable to retrieve agent name "
                                         "at index "
                                      << strAgentIndex << " for Party: " << name
                                      << "\n";
                                return -1;
                            }

                        } else {
                            otOut
                                << "Failed finding the agent's name for party: "
                                << name << " Account: " << acctName
                                << " \n And then failed  finding any agents on "
                                   " this smart contract at ALL.\n";
                            return -1;
                        }
                    }

                    mapIDs[acctName] = acct;
                    mapAgents[acctName] = agentName;

                    accounts--;
                }
            }
        }
    }

    for (auto x = mapIDs.begin(); x != mapIDs.end(); x++) {
        int32_t needed = SwigWrap::SmartContract_CountNumsNeeded(
            contract, mapAgents[x->first]);

        if (!OT::App().API().ServerAction().GetTransactionNumbers(
                Identifier(mynym), Identifier(server), needed + 1)) {
            otOut << "Error: cannot reserve transaction numbers.\n";
            return -1;
        }
    }

    // CONFIRM THE ACCOUNTS HERE
    //
    // Note: Any failure below this point needs to harvest back ALL
    // transaction numbers. Because we haven't even TRIED to activate it,
    // therefore ALL numbers on the contract are still good (even the opening
    // number.)
    //
    // Whereas after a failed activation, we'd need to harvest only the closing
    // numbers, and not the opening numbers. But in here, this is confirmation,
    // not activation.
    string myAcctID = "";
    string myAcctAgentName = "";
    for (auto x = mapIDs.begin(); x != mapIDs.end(); x++) {
        // Here we check to see if MyAcct exists -- if so we compare it to the
        // current acctID and if they match, we set myAcctID. Later on,
        // if/when activating, we can just use myAcctID to activate.
        // (Otherwise we will have to pick one from the confirmed accounts.)
        if ("" == myAcctID && myacct == x->second) {
            myAcctID = myacct;
            myAcctAgentName = mapAgents[x->first];
        }

        // confirm a theoretical acct by giving it a real acct id.
        string confirmed = SwigWrap::SmartContract_ConfirmAccount(
            contract, mynym, name, x->first, mapAgents[x->first], x->second);
        if ("" == confirmed) {
            otOut << "Failure while calling "
                     "OT_API_SmartContract_ConfirmAccount. Acct Name: "
                  << x->first << "  Agent Name: " << mapAgents[x->first]
                  << "  Acct ID: " << x->second << " \n";
            return harvestTxNumbers(contract, mynym);
        }

        contract = confirmed;
    }

    return 1;
}

bool CmdConfirm::showPartyAccounts(
    const string& contract,
    const string& name,
    int32_t depth)
{
    int32_t accounts = SwigWrap::Party_GetAcctCount(contract, name);

    if (0 > accounts) {
        otOut << "Error: Party '" << name
              << "' has bad value for number of asset accounts.\n";
        return false;
    }

    if (0 == depth) {
        cout << "Party '" << name << "' has " << accounts
             << (1 == accounts ? " asset account." : " asset accounts.")
             << "\n";
        return true;
    }

    for (int32_t i = 0; i < accounts; i++) {
        string acctName = SwigWrap::Party_GetAcctNameByIndex(contract, name, i);
        if ("" == acctName) {
            otOut << "Error: Failed retrieving Asset Account Name from party '"
                  << name << "' at account index: " << i << "\n";
            return false;
        }

        string acctInstrumentDefinitionID =
            SwigWrap::Party_GetAcctInstrumentDefinitionID(
                contract, name, acctName);
        if ("" != acctInstrumentDefinitionID) {
            cout << "-------------------\nAccount '" << acctName << "' (index "
                 << i << " on Party '" << name
                 << "') has instrument definition: "
                 << acctInstrumentDefinitionID << " ("
                 << SwigWrap::GetAssetType_Name(acctInstrumentDefinitionID)
                 << ")\n";
        }

        string acctID = SwigWrap::Party_GetAcctID(contract, name, acctName);
        if ("" != acctID) {
            cout << "Account '" << acctName << "' (party '" << name
                 << "') is confirmed as Account ID: " << acctID << " ("
                 << SwigWrap::GetAccountWallet_Name(acctID) << ")\n";
        }

        string strAcctAgentName =
            SwigWrap::Party_GetAcctAgentName(contract, name, acctName);
        if ("" != strAcctAgentName) {
            cout << "Account '" << acctName << "' (party '" << name
                 << "') is managed by agent: " << strAcctAgentName << "\n";
        }
    }

    return true;
}

bool CmdConfirm::showPartyAgents(
    const string& contract,
    const string& name,
    int32_t depth)
{
    int32_t agentCount = SwigWrap::Party_GetAgentCount(contract, name);
    if (0 > agentCount) {
        otOut << "Error: Party '" << name
              << "' has bad value for number of authorized agents.\n";
        return false;
    }

    if (0 == depth) {
        cout << "Party '" << name << "' has " << agentCount
             << (1 == agentCount ? " agent." : " agents.") << "\n";
        return true;
    }

    for (int32_t i = 0; i < agentCount; i++) {
        string agent = SwigWrap::Party_GetAgentNameByIndex(contract, name, i);
        if ("" == agent) {
            otOut << "Error: Failed retrieving Agent Name from party '" << name
                  << "' at agent index: " << i << "\n";
            return false;
        }

        string agentID = SwigWrap::Party_GetAgentID(contract, name, agent);
        if ("" != agentID) {
            cout << "--------------------\n " << i << " : Agent '" << agent
                 << "' (party '" << name << "') has NymID: " << agentID << " ('"
                 << SwigWrap::GetNym_Name(agentID) << "')\n";
        } else {
            cout << " " << i << " : Agent '" << agent << "' (party '" << name
                 << "') has no NymID assigned (yet.)\n";
        }
    }

    return true;
}
