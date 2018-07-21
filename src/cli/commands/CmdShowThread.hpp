// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDSHOWTHREAD_HPP
#define OPENTXS_CLIENT_CMDSHOWTHREAD_HPP

#include "CmdBase.hpp"

#include <chrono>
#include <cstdint>
#include <string>

namespace opentxs
{

class CmdShowThread : public CmdBase
{
public:
    CmdShowThread();

    std::string time(
        const std::chrono::system_clock::time_point in) const;

    std::int32_t run(
        std::string mynym,
        const std::string& threadID);

    ~CmdShowThread() = default;

private:
    std::int32_t runWithOptions() override;
};

} // namespace opentxs

#endif // OPENTXS_CLIENT_CMDSHOWTHREAD_HPP
