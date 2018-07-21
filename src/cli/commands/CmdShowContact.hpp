// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDSHOWCONTACT_HPP
#define OPENTXS_CLIENT_CMDSHOWCONTACT_HPP

#include "CmdBase.hpp"

#include <cstdint>
#include <string>

namespace opentxs
{

class CmdShowContact : public CmdBase
{
public:
    CmdShowContact();

    std::int32_t run(const std::string& id);

    ~CmdShowContact() = default;

private:
    void display_groups(const ui::ContactSection& section) const;
    void display_items(const ui::ContactSubsection& group) const;
    std::int32_t runWithOptions() override;
};
} // namespace opentxs

#endif // OPENTXS_CLIENT_CMDSHOWCONTACT_HPP
