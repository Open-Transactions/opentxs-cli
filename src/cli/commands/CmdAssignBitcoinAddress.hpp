// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDASSIGNBITCOINADDRESS_HPP
#define OPENTXS_CLIENT_CMDASSIGNBITCOINADDRESS_HPP

#include "CmdBase.hpp"

#include <cstdint>
#include <string>

namespace opentxs
{

class CmdAssignBitcoinAddress : public CmdBase
{
protected:
    std::int32_t runWithOptions() override;

public:
    EXPORT CmdAssignBitcoinAddress();

    EXPORT std::int32_t run(
        std::string mynym,
        const std::string& account,
        const std::string& chain,
        const std::string& index,
        const std::string& contact);

    EXPORT ~CmdAssignBitcoinAddress() = default;
};
} // namespace opentxs

#endif // OPENTXS_CLIENT_CMDASSIGNBITCOINADDRESS_HPP
