// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDMERGECONTACT_HPP
#define OPENTXS_CLIENT_CMDMERGECONTACT_HPP

#include "CmdBase.hpp"

#include <cstdint>
#include <string>

namespace opentxs
{

class CmdMergeContact : public CmdBase
{
public:
    CmdMergeContact();

    std::int32_t run(const std::string& parent, const std::string& child);

    ~CmdMergeContact() = default;

private:
    std::int32_t runWithOptions() override;
};
} // namespace opentxs

#endif // OPENTXS_CLIENT_CMDMERGECONTACT_HPP
