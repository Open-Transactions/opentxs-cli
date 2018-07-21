// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDNEWASSET_HPP
#define OPENTXS_CLIENT_CMDNEWASSET_HPP

#include "CmdBase.hpp"

namespace opentxs
{

class CmdNewAsset : public CmdBase
{
public:
    EXPORT CmdNewAsset();
    virtual ~CmdNewAsset();

    EXPORT int32_t run(
        std::string mynym,
        std::string shortname,
        std::string name,
        std::string symbol,
        std::string tla,
        std::string power,
        std::string fraction);

protected:
    virtual int32_t runWithOptions();
};

} // namespace opentxs

#endif // OPENTXS_CLIENT_CMDNEWASSET_HPP
