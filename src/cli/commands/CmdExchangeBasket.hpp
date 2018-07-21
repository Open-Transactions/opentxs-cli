// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDEXCHANGEBASKET_HPP
#define OPENTXS_CLIENT_CMDEXCHANGEBASKET_HPP

#include "CmdBase.hpp"

namespace opentxs
{

class CmdExchangeBasket : public CmdBase
{
public:
    EXPORT CmdExchangeBasket();
    virtual ~CmdExchangeBasket();

    EXPORT int32_t
        run(std::string myacct, std::string direction, std::string multiple);

protected:
    std::int32_t runWithOptions() override;

private:
    int32_t showBasketAccounts(const std::string& server,
                               const std::string& mynym,
                               const std::string& assetType, bool bFilter);
};

} // namespace opentxs

#endif // OPENTXS_CLIENT_CMDEXCHANGEBASKET_HPP
