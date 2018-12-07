// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdImportCash.hpp"

#include <opentxs/opentxs.hpp>

#include <ostream>
#include <stdint.h>
#include <string>

using namespace opentxs;
using namespace std;

CmdImportCash::CmdImportCash()
{
    command = "importcash";
    args[0] = "[--mynym <nym>]";
    category = catInstruments;
    help = "Import a pasted cash purse.";
    usage = "Specify mynym when a signer nym cannot be deduced.";
}

CmdImportCash::~CmdImportCash() {}

int32_t CmdImportCash::runWithOptions() { return run(getOption("mynym")); }

int32_t CmdImportCash::run(string mynym)
{
#if OT_CASH
#define OT_METHOD "opentxs::CmdImportCash::"

    if ("" != mynym && !checkNym("mynym", mynym)) { return -1; }

    string instrument = inputText("a cash purse");
    if ("" == instrument) { return -1; }

    string type = SwigWrap::Instrmnt_GetType(instrument);
    if ("" == type) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot determine instrument type.")
            .Flush();
        return -1;
    }

    string server = SwigWrap::Instrmnt_GetNotaryID(instrument);
    if ("" == server) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot determine instrument server.")
            .Flush();
        return -1;
    }

    if ("PURSE" != type) {
        // Todo: case "TOKEN"
        //
        // NOTE: This is commented out because since it is guessing the NymID as
        // MyNym, then it will just create a purse for MyNym and import it into
        // that purse, and then later when doing a deposit, THAT's when it tries
        // to DECRYPT that token and re-encrypt it to the SERVER's nym... and
        // that's when we might find out that it never was encrypted to MyNym in
        // the first place -- we had just assumed it was here, when we did the
        // import. Until I can look at that in more detail, it will remain
        // commented out.
        // bool bImportedToken = importCashPurse(server, mynym,
        // instrumentDefinitionID,
        //                                      userInput, isPurse);
        // if (importCashPurse(server, mynym, instrumentDefinitionID, userInput,
        // isPurse))
        //{
        //    otOut << "\n\n Success importing cash token!\nServer: "
        //        << server << "\nAsset Type: " << instrumentDefinitionID
        //        << "\nNym: " << MyNym << "\n\n";
        //    return 1;
        //}

        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: invalid instrument type. Expected PURSE.")
            .Flush();
        return -1;
    }

    string purseOwner = "";
    if (!SwigWrap::Purse_HasPassword(server, instrument)) {
        purseOwner = SwigWrap::Instrmnt_GetRecipientNymID(instrument);
    }

    // Whether the purse was password-protected (and thus had no Nym ID) or
    // whether it does have a Nym ID (but it wasn't listed on the purse)
    // Then either way, in those cases purseOwner will still be empty.
    //
    // (The third case is that the purse is Nym protected and the ID WAS
    // available, in which case we'll skip this block, since we already
    // have it.)
    //
    // But even in the case where there's no Nym at all (password protected)
    // we STILL need to pass a Signer Nym ID into
    // SwigWrap::Wallet_ImportPurse.
    // So if it's still empty here, then we use --mynym to make the call.
    // And also, even in the case where there IS a Nym but it's not listed,
    // we must assume the USER knows the appropriate NymID, even if it's not
    // listed on the purse itself. And in that case as well, the user can
    // simply specify the Nym using --mynym.
    //
    // Bottom line: by this point, if it's still not set, then we just use
    // MyNym, and if THAT's not set, then we return failure.
    if ("" == purseOwner) {
        purseOwner = mynym;
        if ("" == purseOwner) {
            LogNormal(OT_METHOD)(__FUNCTION__)(
                ": Error: cannot determine purse owner."
                "Please specify mynym.")
                .Flush();
            return -1;
        }
    }

    string instrumentDefinitionID =
        SwigWrap::Instrmnt_GetInstrumentDefinitionID(instrument);
    if ("" == instrumentDefinitionID) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot determine "
                                           "instrument definition ID.")
            .Flush();
        return -1;
    }

    if (!SwigWrap::Wallet_ImportPurse(
            server, instrumentDefinitionID, purseOwner, instrument)) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot import purse.")
            .Flush();
        return -1;
    }

    return 1;
#else
    return -1;
#endif  // OT_CASH
}
