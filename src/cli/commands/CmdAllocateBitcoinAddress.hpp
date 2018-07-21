// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDALLOCATEBITCOINADDRESS_HPP
#define OPENTXS_CLIENT_CMDALLOCATEBITCOINADDRESS_HPP

#include "CmdBase.hpp"

#include <cstdint>
#include <string>

namespace opentxs
{

class CmdAllocateBitcoinAddress : public CmdBase
{
protected:
    std::int32_t runWithOptions() override;

public:
    EXPORT CmdAllocateBitcoinAddress();

    EXPORT std::int32_t run(
        std::string mynym,
        const std::string& account,
        const std::string& label,
        const std::string& chain);

    EXPORT ~CmdAllocateBitcoinAddress() = default;
};
} // namespace opentxs

#endif // OPENTXS_CLIENT_CMDALLOCATEBITCOINADDRESS_HPP
