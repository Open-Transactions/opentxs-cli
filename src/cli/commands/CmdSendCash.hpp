// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDSENDCASH_HPP
#define OPENTXS_CLIENT_CMDSENDCASH_HPP

#include "CmdBaseInstrument.hpp"

namespace opentxs
{

class CmdSendCash : public CmdBaseInstrument
{
public:
    EXPORT CmdSendCash();
    virtual ~CmdSendCash();

    EXPORT int32_t run(
        std::string server,
        std::string mynym,
        std::string myacct,
        std::string mypurse,
        std::string hisnym,
        std::string amount,
        std::string indices,
        std::string password);
    EXPORT int32_t sendCash(
        std::string& response,
        const std::string& server,
        const std::string& mynym,
        const std::string& assetType,
        const std::string& myacct,
        std::string& hisnym,
        const std::string& amount,
        std::string& indices,
        bool hasPassword) const;

protected:
    int32_t runWithOptions() override;
};

}  // namespace opentxs

#endif  // OPENTXS_CLIENT_CMDSENDCASH_HPP
