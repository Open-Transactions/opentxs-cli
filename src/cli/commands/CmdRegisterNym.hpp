// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDREGISTERNYM_HPP
#define OPENTXS_CLIENT_CMDREGISTERNYM_HPP

#include "CmdBase.hpp"

#include <cstdint>
#include <string>

namespace opentxs
{

class CmdRegisterNym : public CmdBase
{
public:
    EXPORT CmdRegisterNym();
    EXPORT ~CmdRegisterNym() = default;

    EXPORT std::int32_t run(
        std::string server,
        std::string mynym,
        std::string publish,
        std::string primary);

protected:
    std::int32_t runWithOptions() override;
};

}  // namespace opentxs

#endif  // OPENTXS_CLIENT_CMDREGISTERNYM_HPP
