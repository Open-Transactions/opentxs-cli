// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDWITHDRAWCASH_HPP
#define OPENTXS_CLIENT_CMDWITHDRAWCASH_HPP

#include "CmdBase.hpp"

namespace opentxs
{

class CmdWithdrawCash : public CmdBase
{
public:
    EXPORT CmdWithdrawCash();
    virtual ~CmdWithdrawCash();

    EXPORT int32_t run(std::string myacct, std::string amount);

    EXPORT int32_t
    withdrawCash(const std::string& myacct, int64_t amount) const;

protected:
    int32_t runWithOptions() override;
};

}  // namespace opentxs

#endif  // OPENTXS_CLIENT_CMDWITHDRAWCASH_HPP
