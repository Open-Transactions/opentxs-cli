// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDNEWOFFER_HPP
#define OPENTXS_CLIENT_CMDNEWOFFER_HPP

#include "CmdBase.hpp"

namespace opentxs
{

class CmdNewOffer : public CmdBase
{
public:
    EXPORT CmdNewOffer();
    virtual ~CmdNewOffer();

    EXPORT int32_t
    run(std::string mynym,
        std::string hisnym,
        std::string type,
        std::string scale,
        std::string mininc,
        std::string quantity,
        std::string price,
        std::string lifespan);

protected:
    std::int32_t runWithOptions() override;

private:
    int32_t cleanMarketOfferList(
        const std::string& server,
        const std::string& mynym,
        const std::string& myacct,
        const std::string& hisacct,
        const std::string& type,
        const std::string& scale,
        const std::string& price);
};

}  // namespace opentxs

#endif  // OPENTXS_CLIENT_CMDNEWOFFER_HPP
