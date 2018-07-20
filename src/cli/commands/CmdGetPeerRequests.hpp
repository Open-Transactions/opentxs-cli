// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDGETPEERREQUESTS_HPP
#define OPENTXS_CLIENT_CMDGETPEERREQUESTS_HPP

#include "CmdBase.hpp"

#include <cstdint>
#include <string>

namespace opentxs
{

class CmdGetPeerRequests : public CmdBase
{
public:
    EXPORT CmdGetPeerRequests();
    virtual ~CmdGetPeerRequests();

    EXPORT std::int32_t
        run(std::string mynym);

protected:
    virtual std::int32_t runWithOptions();
};

} // namespace opentxs

#endif // OPENTXS_CLIENT_CMDGETPEERREQUESTS_HPP
