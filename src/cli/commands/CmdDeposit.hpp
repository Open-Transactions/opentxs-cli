// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDDEPOSIT_HPP
#define OPENTXS_CLIENT_CMDDEPOSIT_HPP

#include "CmdBaseInstrument.hpp"

namespace opentxs
{

class CmdDeposit : public CmdBaseInstrument
{
public:
    EXPORT CmdDeposit();
    virtual ~CmdDeposit();

    EXPORT int32_t
    run(std::string mynym, std::string myacct, std::string indices);

    EXPORT int32_t depositCheque(
        const std::string& server,
        const std::string& myacct,
        const std::string& mynym,
        const std::string& instrument,
        std::string* pOptionalOutput = nullptr) const;
    EXPORT int32_t depositPurse(
        const std::string& server,
        const std::string& myacct,
        const std::string& mynym,
        std::string instrument,
        const std::string& indices,
        std::string* pOptionalOutput = nullptr) const;

protected:
    int32_t runWithOptions() override;
};

}  // namespace opentxs

#endif  // OPENTXS_CLIENT_CMDDEPOSIT_HPP
