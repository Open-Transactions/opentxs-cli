// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDSHOWMARKETS_HPP
#define OPENTXS_CLIENT_CMDSHOWMARKETS_HPP

#include "CmdBase.hpp"

namespace opentxs
{

namespace OTDB
{
class MarketList;
}

class CmdShowMarkets : public CmdBase
{
public:
    EXPORT CmdShowMarkets();
    virtual ~CmdShowMarkets();

    EXPORT int32_t run(std::string server);

protected:
    virtual int32_t runWithOptions();

private:
    OTDB::MarketList* loadMarketList(const std::string& server);
};

} // namespace opentxs

#endif // OPENTXS_CLIENT_CMDSHOWMARKETS_HPP
