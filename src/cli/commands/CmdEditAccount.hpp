// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDEDITACCOUNT_HPP
#define OPENTXS_CLIENT_CMDEDITACCOUNT_HPP

#include "CmdBase.hpp"

namespace opentxs
{

class CmdEditAccount : public CmdBase
{
public:
    EXPORT CmdEditAccount();
    virtual ~CmdEditAccount();

    EXPORT int32_t run(std::string myacct, std::string label);

protected:
    virtual int32_t runWithOptions();
};

} // namespace opentxs

#endif // OPENTXS_CLIENT_CMDEDITACCOUNT_HPP
