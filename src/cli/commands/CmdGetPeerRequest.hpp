// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDGETPEERREQUEST_HPP
#define OPENTXS_CLIENT_CMDGETPEERREQUEST_HPP

#include "CmdBase.hpp"

#include <cstdint>
#include <string>

namespace opentxs
{

class CmdGetPeerRequest : public CmdBase
{
protected:
    std::int32_t runWithOptions() override;

public:
    EXPORT CmdGetPeerRequest();

    EXPORT std::int32_t run(std::string mynym, std::string request);

    EXPORT ~CmdGetPeerRequest() = default;
};

} // namespace opentxs

#endif // OPENTXS_CLIENT_CMDGETPEERREQUEST_HPP
