// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDBASEINSTRUMENT_HPP
#define OPENTXS_CLIENT_CMDBASEINSTRUMENT_HPP

#include "CmdBase.hpp"

namespace opentxs
{
class CmdBaseInstrument : public CmdBase
{
protected:
    CmdBaseInstrument();
    virtual ~CmdBaseInstrument() = 0;

    int32_t sendPayment(
        const std::string& cheque,
        std::string sender,
        const char* what) const;
    std::string writeCheque(
        std::string myacct,
        std::string hisnym,
        std::string amount,
        std::string memo,
        std::string validfor,
        bool isInvoice) const;
};

}  // namespace opentxs

#endif  // OPENTXS_CLIENT_CMDBASEINSTRUMENT_HPP
