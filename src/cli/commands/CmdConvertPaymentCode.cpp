// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdConvertPaymentCode.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>

#define OT_METHOD "opentxs::CmdConvertPaymentCode::"

namespace opentxs
{

CmdConvertPaymentCode::CmdConvertPaymentCode()
{
    command = "convertpaymentcode";
    args[0] = "--code <payment code>";
    category = catMisc;
    help = "Convert a BIP-47 payment code to a nym ID";
}

std::int32_t CmdConvertPaymentCode::runWithOptions()
{
    return run(getOption("code"));
}

std::int32_t CmdConvertPaymentCode::run(const std::string& code)
{
    if (!checkMandatory("code", code)) { return -1; }

    const std::string id = SwigWrap::NymIDFromPaymentCode(code);

    if ("" == id) {
        LogNormal(OT_METHOD)(__FUNCTION__)(": Error: invalid payment code.")
            .Flush();

        return -1;
    }

    LogNormal(OT_METHOD)(__FUNCTION__)(": ")(id)(".").Flush();

    return 1;
}
}  // namespace opentxs
