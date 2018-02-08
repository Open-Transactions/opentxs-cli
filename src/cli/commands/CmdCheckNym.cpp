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

#include "CmdCheckNym.hpp"

#include "CmdBase.hpp"

#include <opentxs/api/Api.hpp>
#include <opentxs/api/Native.hpp>
#include <opentxs/client/OT_ME.hpp>
#include <opentxs/OT.hpp>

#include <stdint.h>
#include <string>

using namespace opentxs;
using namespace std;

CmdCheckNym::CmdCheckNym()
{
    command = "checknym";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    args[2] = "--hisnym <nym>";
    category = catOtherUsers;
    help = "Download hisnym's public key.";
}

CmdCheckNym::~CmdCheckNym() {}

int32_t CmdCheckNym::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"), getOption("hisnym"));
}

int32_t CmdCheckNym::run(string server, string mynym, string hisnym)
{
    if (!checkServer("server", server)) {
        return -1;
    }

    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    if (!checkNym("hisnym", hisnym, false)) {
        return -1;
    }

    /*
     NOTE: This is a very strange case!

     Most messages, if they return false, mean it failed.
     Well in this case, if it returns success=false, it means
     it failed to find the Nym you are checking.

     Why is this important? Normally if you think a Nym is on a notary,
     it's because he actually is. (Maybe he sent you a message or something,
     using that notary, and now you want to reply to him, so you check_nym
     so you can download his public key.)
     In this case, if the message returns false, the CLI will correctly announce
     that something has "failed." OT normally does a few re-tries in this sort
     of case. It thinks maybe the request number is out of synch, so it re-syncs
     and then tries again.

     ==> BUT! what if the NymID you are checking REALLY ISN'T ON THAT SERVER?

     ===> In that case, it will return "success=false". But it's NOT telling you
     that some sort of error occurred, or that you are out of synch. Rather,
     it's
     (correctly) informing you that you are trying to download a pubkey for a
     Nym
     you really actually wasn't found on that server!

     ===> I don't see a fix for this, except to change the checkNym message so
     that
     the success=true and some new variable foundNym=false. (TODO.)

     */

    string response = OT::App().API().OTME().check_nym(server, mynym, hisnym);
    return processResponse(response, "check nym");
}
