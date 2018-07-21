// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDSHOWPROFILE_HPP
#define OPENTXS_CLIENT_CMDSHOWPROFILE_HPP

#include "CmdBase.hpp"

#include <cstdint>
#include <string>

namespace opentxs
{

class CmdShowProfile : public CmdBase
{
public:
    CmdShowProfile();

    std::int32_t run(std::string mynym);

    ~CmdShowProfile() = default;

private:
    void display_groups(const ui::ProfileSection& section) const;
    void display_items(const ui::ProfileSubsection& group) const;
    std::int32_t runWithOptions() override;
};
} // namespace opentxs
#endif // OPENTXS_CLIENT_CMDSHOWPROFILE_HPP
