// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdProposePlan.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdProposePlan::CmdProposePlan()
{
    command = "proposeplan";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--hisnym <nym>";
    args[3] = "--myacct <account>";
    args[4] = "--hisacct <account>";
    args[5] = "[--memo <consideration>]";
    args[6] = "[--daterange <start,length>]";
    args[7] = "[--initialpayment <amount,delay>";
    args[8] = "[--paymentplan <amount,delay,period>";
    args[9] = "[--planexpiry <length,number>";
    category = catInstruments;
    help = "as merchant, propose a payment plan to a customer.";
    usage = "Daterange: <start> default is the current time,\n"
            "           <length> default is 'no expiry.'\n"
            "Initialpayment <amount> default is 0 (no initial payment),\n"
            "               <delay> in seconds, from creation date.\n"
            "Paymentplan <amount> is a recurring payment,\n"
            "            <delay> in seconds, default is 30 days,\n"
            "            <period> in seconds, default is 30 days.\n"
            "Planexpiry <length> in seconds, default is unlimited,\n"
            "           <number> of payments, default is unlimited.";
}

CmdProposePlan::~CmdProposePlan() {}

int32_t CmdProposePlan::runWithOptions()
{
    return run(
        getOption("server"),
        getOption("mynym"),
        getOption("hisnym"),
        getOption("myacct"),
        getOption("hisacct"),
        getOption("memo"),
        getOption("daterange"),
        getOption("initialpayment"),
        getOption("paymentplan"),
        getOption("planexpiry"));
}

// The merchant (sender of proposal / payee of proposal) proposes the payment
// plan to the customer (recipient of proposal / payer of proposal.)
// So this function is called by the merchant.

int32_t CmdProposePlan::run(
    string server,
    string mynym,
    string hisnym,
    string myacct,
    string hisacct,
    string memo,
    string daterange,
    string initialpayment,
    string paymentplan,
    string planexpiry)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    if (!checkNym("hisnym", hisnym)) { return -1; }

    if (!checkAccount("myacct", myacct)) { return -1; }

    if (!checkAccount("hisacct", hisacct)) { return -1; }

    if ("" == memo) {
        memo = inputText("a consideration");
        if ("" == memo) { return -1; }
    }

    otOut << "date_range (from,to): " << daterange << "\n";
    otOut << "consideration: " << memo << "\n";
    otOut << "initial_payment (amount,delay): " << initialpayment << "\n";
    otOut << "payment_plan (amount,delay,period): " << paymentplan << "\n";
    otOut << "plan_expiry (length,number): " << planexpiry << "\n";

    {
        if (!OT::App().Client().ServerAction().GetTransactionNumbers(
                Identifier::Factory(mynym), Identifier::Factory(server), 2)) {
            otOut << "Error: cannot reserve transaction numbers.\n";
            return -1;
        }
    }

    // SwigWrap::EasyProposePlan is a version of ProposePaymentPlan that
    // compresses it into a fewer number of arguments. (Then it expands them
    // and calls ProposePaymentPlan.)
    // Basically this version has ALL the same parameters, but it stuffs two or
    // three at a time into a single parameter, as a comma-separated list in
    // string form. See details for each parameter, in the comment below.
    string plan = SwigWrap::EasyProposePlan(
        server,
        daterange,
        hisacct,
        hisnym,
        memo,
        myacct,
        mynym,
        initialpayment,
        paymentplan,
        planexpiry);
    if ("" == plan) {
        otOut << "Error: cannot create proposed plan.\n";
        return -1;
    }

    // The "propose" step (performed by the merchant) includes the merchant's
    // confirmation / signing.
    // (So there's no need for the merchant to additionally "confirm" the plan
    // before sending it -- he already has done that by this point, just as part
    // of the proposal itself.)

    auto payment = std::make_shared<const OTPayment>(
        OT::App().Legacy().ClientDataFolder(), String(plan.c_str()));
    std::string response;
    {
        response = OT::App()
                       .Client()
                       .ServerAction()
                       .SendPayment(
                           Identifier::Factory(mynym),
                           Identifier::Factory(server),
                           Identifier::Factory(hisnym),
                           payment)
                       ->Run();
    }
    if (1 != responseStatus(response)) {
        otOut << "Error: cannot send payment plan.\n";
        return harvestTxNumbers(plan, mynym);
    }

    return 1;
}
