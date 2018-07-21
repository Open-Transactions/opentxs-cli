// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDSHOWPURSE_HPP
#define OPENTXS_CLIENT_CMDSHOWPURSE_HPP

#include "CmdBase.hpp"

namespace opentxs
{

class CmdShowPurse : public CmdBase
{
public:
    EXPORT CmdShowPurse();
    virtual ~CmdShowPurse();

    EXPORT int32_t
        run(std::string server, std::string mynym, std::string mypurse);

protected:
    virtual int32_t runWithOptions();
};

} // namespace opentxs

#endif // OPENTXS_CLIENT_CMDSHOWPURSE_HPP
