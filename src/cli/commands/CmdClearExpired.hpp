// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDCLEAREXPIRED_HPP
#define OPENTXS_CLIENT_CMDCLEAREXPIRED_HPP

#include "CmdBase.hpp"

namespace opentxs
{

class CmdClearExpired : public CmdBase
{
public:
    EXPORT CmdClearExpired();
    virtual ~CmdClearExpired();

    EXPORT int32_t run(std::string server, std::string mynym);

protected:
    virtual int32_t runWithOptions();
};

} // namespace opentxs

#endif // OPENTXS_CLIENT_CMDCLEAREXPIRED_HPP
