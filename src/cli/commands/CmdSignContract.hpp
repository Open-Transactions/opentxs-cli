// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDSIGNCONTRACT_HPP
#define OPENTXS_CLIENT_CMDSIGNCONTRACT_HPP

#include "CmdBase.hpp"

namespace opentxs
{

class CmdSignContract : public CmdBase
{
public:
    EXPORT CmdSignContract();
    virtual ~CmdSignContract();

    EXPORT int32_t run(std::string mynym, std::string type);

protected:
    virtual int32_t runWithOptions();
};

} // namespace opentxs

#endif // OPENTXS_CLIENT_CMDSIGNCONTRACT_HPP
