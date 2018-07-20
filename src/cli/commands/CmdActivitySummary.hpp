// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDACTIVITYSUMMARY_HPP
#define OPENTXS_CLIENT_CMDACTIVITYSUMMARY_HPP

#include "CmdBase.hpp"

#include <chrono>
#include <cstdint>
#include <string>

namespace opentxs
{

class CmdActivitySummary : public CmdBase
{
public:
    CmdActivitySummary();

    std::int32_t run(std::string mynym);

    ~CmdActivitySummary() = default;

private:
    std::string time(const std::chrono::system_clock::time_point in) const;

    std::int32_t runWithOptions() override;
};
} // namespace opentxs
#endif // OPENTXS_CLIENT_CMDACTIVITYSUMMARY_HPP
