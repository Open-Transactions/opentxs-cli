// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDSHOWMINT_HPP
#define OPENTXS_CLIENT_CMDSHOWMINT_HPP

#include "CmdBase.hpp"

namespace opentxs
{

class CmdShowMint : public CmdBase
{
public:
    EXPORT CmdShowMint();
    virtual ~CmdShowMint();

    EXPORT int32_t
        run(std::string server, std::string mynym, std::string mypurse);

protected:
    int32_t runWithOptions() override;

    std::string load_or_retrieve_mint(
        const std::string& notaryID,
        const std::string& nymID,
        const std::string& unitTypeID) const;
};

} // namespace opentxs

#endif // OPENTXS_CLIENT_CMDSHOWMINT_HPP
