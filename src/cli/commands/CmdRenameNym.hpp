// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDRENAMENYM_HPP
#define OPENTXS_CLIENT_CMDRENAMENYM_HPP

#include "CmdBase.hpp"

namespace opentxs
{

class CmdRenameNym : public CmdBase
{
protected:
    std::int32_t runWithOptions() override;

public:
    EXPORT CmdRenameNym();

    EXPORT std::int32_t run(std::string mynym, std::string label);

    EXPORT ~CmdRenameNym() = default;
};

} // namespace opentxs

#endif // OPENTXS_CLIENT_CMDRENAMENYM_HPP
