// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdTriggerClause.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <string>

using namespace opentxs;
using namespace std;

CmdTriggerClause::CmdTriggerClause()
{
    command = "triggerclause";
    // FIX many more args
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--id <transactionnr>";
    args[3] = "--clause <clausename>";
    args[4] = "[--param <parameter>]";
    category = catInstruments;
    help = "Trigger a clause on a running smart contract.";
}

CmdTriggerClause::~CmdTriggerClause() {}

int32_t CmdTriggerClause::runWithOptions()
{
    return run(
        getOption("server"),
        getOption("mynym"),
        getOption("id"),
        getOption("clause"),
        getOption("param"));
}

// We might also need something like: OT_Command::main_show_cron_items
// (Because we can't trigger a clause on a running smart contract, unless
// we are able to list the running smart contracts and thus ascertain
// the transaction number of the one whose clause we wish to trigger.)

int32_t CmdTriggerClause::run(
    string server,
    string mynym,
    string id,
    string clause,
    string param)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    if (!checkTransNum("id", id)) { return -1; }

    if (!checkMandatory("clause", clause)) { return -1; }

    if ("" == param) {
        param = inputText("the clause parameter");
        if ("" == param) { return -1; }
    }

    std::string response;
    {
        response = Opentxs::
                       Client()
                       .ServerAction()
                       .TriggerClause(
                           Identifier::Factory(mynym),
                           Identifier::Factory(server),
                           std::stoi(id),
                           clause,
                           param)
                       ->Run();
    }
    return processResponse(response, "trigger clause");
}
