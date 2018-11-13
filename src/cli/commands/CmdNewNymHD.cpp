// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdNewNymHD.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

#define OT_METHOD "opentxs::CmdNewNymHD::"

using namespace opentxs;
using namespace std;

CmdNewNymHD::CmdNewNymHD()
{
    command = "newnymhd";
    args[0] = "--label <label>";
    args[1] = "[--source <seed fingerprint>]";
    args[2] = "[--index <HD derivation path>]";
    category = catNyms;
    help = "create a new nym using HD key derivation.";
}

int32_t CmdNewNymHD::runWithOptions()
{
    return run(getOption("label"), getOption("source"), getOption("index"));
}

int32_t CmdNewNymHD::run(string label, string source, string path)
{
    if (!checkMandatory("label", label)) { return -1; }

    std::int32_t nym = -1;

    if (!path.empty()) {
        try {
            nym = stol(path);
        } catch (const std::invalid_argument&) {
            nym = -1;
        } catch (const std::out_of_range&) {
            nym = -1;
        }

        const std::uint32_t hardened =
            static_cast<std::uint32_t>(opentxs::Bip32Child::HARDENED);

        if (nym > 0 && nym >= (int32_t)hardened) {
            nym = nym ^ (int32_t)hardened;
        }
    }

    std::string mynym = SwigWrap::CreateIndividualNym(label, source, nym);

    if ("" == mynym) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: cannot create new nym.")
            .Flush();
        return -1;
    }

    cout << "New nym: " << mynym << "\n";

    return 1;
}
