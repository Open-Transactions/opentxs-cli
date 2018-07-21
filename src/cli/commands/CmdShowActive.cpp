// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdShowActive.hpp"

#include <opentxs/opentxs.hpp>

#include <stddef.h>
#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>

using namespace opentxs;
using namespace std;

CmdShowActive::CmdShowActive()
{
    command = "showactive";
    args[0] = "--server <server>";
    args[1] = "[--mynym <nym>]";
    args[2] = "[--id <transactionnr>]";
    category = catInstruments;
    help = "Show the active cron item IDs, or the details of one by ID.";
    usage = "Specify either of --mynym and --id.";
}

CmdShowActive::~CmdShowActive() {}

int32_t CmdShowActive::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("id"));
}

int32_t CmdShowActive::run(string server, string mynym, string id)
{
    if (!checkServer("server", server)) { return -1; }

    // optional, specific transaction id
    if ("" != id) {
        int64_t transNum = checkTransNum("id", id);
        if (0 > transNum) { return -1; }

        // FIX: what about error reporting here?
        string item = SwigWrap::GetActiveCronItem(server, transNum);
        if ("" != item) {
            string type = SwigWrap::Instrmnt_GetType(item);
            if ("" == type) { type = "UNKNOWN"; }

            otOut << "Found an active transaction!\n"
                  << "ID: " << transNum << "  Type: " << type
                  << "\n\nContents:\n\n"
                  << item << "\n\n";
        }
        return 1;
    }

    if (!checkNym("mynym", mynym)) { return -1; }

    string ids = SwigWrap::GetNym_ActiveCronItemIDs(mynym, server);
    if ("" == ids) {
        otOut << "Found no active transactions. "
                 "Perhaps try 'opentxs refresh' first?\n";
        return 0;
    }

    vector<string> items = tokenize(ids, ',', true);
    otOut << "\n Found " << items.size() << " active transactions:  " << ids
          << "\n\n";
    for (size_t i = 0; i < items.size(); i++) {
        string id = items[i];
        int64_t transNum = checkTransNum("id", id);
        if (0 < transNum) {
            // FIX: what about error reporting here?
            string item = SwigWrap::GetActiveCronItem(server, transNum);
            if ("" != item) {
                string type = SwigWrap::Instrmnt_GetType(item);
                if ("" == type) { type = "UNKNOWN"; }
                cout << "ID: " << transNum << "  Type: " << type << "\n\n";
            }
        }
    }

    return 1;
}
