// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDSHOWISSUERS_HPP
#define OPENTXS_CLIENT_CMDSHOWISSUERS_HPP

#include "CmdBase.hpp"

#include <cstdint>

namespace opentxs
{

class CmdShowIssuers : public CmdBase
{
public:
    EXPORT CmdShowIssuers();

    EXPORT std::int32_t run(std::string mynym, std::string currency);

    EXPORT ~CmdShowIssuers() = default;

private:
    void print_accounts(const opentxs::ui::IssuerItem& line) const;
    void print_line(const opentxs::ui::IssuerItem& line) const;
    void print_line(const opentxs::ui::AccountSummaryItem& line) const;

    std::int32_t runWithOptions() override;
};
} // namespace opentxs
#endif // OPENTXS_CLIENT_CMDSHOWISSUERS_HPP
