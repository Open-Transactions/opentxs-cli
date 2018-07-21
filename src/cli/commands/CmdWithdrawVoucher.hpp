// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDWITHDRAWVOUCHER_HPP
#define OPENTXS_CLIENT_CMDWITHDRAWVOUCHER_HPP

#include "CmdBase.hpp"

namespace opentxs
{

class CmdWithdrawVoucher : public CmdBase
{
public:
    EXPORT CmdWithdrawVoucher();
    virtual ~CmdWithdrawVoucher();

    EXPORT int32_t run(std::string myacct, std::string hisnym,
                       std::string amount, std::string memo);

    EXPORT int32_t run(std::string myacct, std::string hisnym,
                       std::string amount, std::string memo,
                       std::string& voucher);

protected:
    std::int32_t runWithOptions() override;
};

} // namespace opentxs

#endif // OPENTXS_CLIENT_CMDWITHDRAWVOUCHER_HPP
