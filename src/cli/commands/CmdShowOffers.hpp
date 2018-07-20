// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDSHOWOFFERS_HPP
#define OPENTXS_CLIENT_CMDSHOWOFFERS_HPP

#include "CmdBase.hpp"

namespace opentxs
{

namespace OTDB
{
class OfferListMarket;
}

class CmdShowOffers : public CmdBase
{
public:
    EXPORT CmdShowOffers();
    virtual ~CmdShowOffers();

    EXPORT int32_t run(std::string server, std::string market);

    EXPORT int32_t show(const std::string& server, const std::string& market);

protected:
    virtual int32_t runWithOptions();

private:
    OTDB::OfferListMarket* loadMarketOffers(const std::string& server,
                                            const std::string& market);
};

} // namespace opentxs

#endif // OPENTXS_CLIENT_CMDSHOWOFFERS_HPP
