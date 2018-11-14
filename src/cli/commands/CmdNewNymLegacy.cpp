// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdNewNymLegacy.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

#define OT_METHOD "opentxs::CmdNewNymLegacy::"

using namespace opentxs;
using namespace std;

CmdNewNymLegacy::CmdNewNymLegacy()
{
    command = "newnymlegacy";
    args[0] = "--label <label>";
    args[1] = "[--keybits <1024|2048|4096|8192>]";
    args[2] = "[--source <source>]";
    args[3] = "[--location <location>]";
    category = catNyms;
    help = "create a new OpenSSL-based RSA nym.";
}

CmdNewNymLegacy::~CmdNewNymLegacy() {}

int32_t CmdNewNymLegacy::runWithOptions()
{
    return run(getOption("keybits"), getOption("label"), getOption("source"));
}

// FYI, a source can be a URL, a Bitcoin address, a Namecoin address,
// a public key, or the unique DN info from a traditionally-issued cert.
// Hashing the source should produce the NymID. Also, the source should
// always (somehow) validate the credential IDs, if they are to be trusted
// for their purported Nym.
//
// NOTE: If you leave the source BLANK, then OT will just generate a public
// key to serve as the source. The public key will be hashed to form the
// NymID, and all credentials for that Nym will need to be signed by the
// corresponding private key. That's the only way they can be 'verified by
// their source.'

int32_t CmdNewNymLegacy::run(string keybits, string label, string source)
{
    if (!checkMandatory("label", label)) { return -1; }

    if ("" != keybits && !checkValue("keybits", keybits)) { return -1; }

    int32_t bits = "" == keybits ? 1024 : stoi(keybits);
    if (1024 != bits && 2048 != bits && 4096 != bits && 8192 != bits) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: invalid keybits value.")
            .Flush();
        return -1;
    }

    string mynym = SwigWrap::CreateNymLegacy(bits, source);
    if ("" == mynym) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot create new nym.")
            .Flush();
        return -1;
    }

    cout << "New nym: " << mynym << "\n";

    if (!SwigWrap::SetNym_Alias(mynym, mynym, label)) {
        LogNormal(OT_METHOD)(__FUNCTION__)(
            ": Error: cannot create new nym name.")
            .Flush();
        return -1;
    }

    return 1;
}
