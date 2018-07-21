// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDREFRESHNYM_HPP
#define OPENTXS_CLIENT_CMDREFRESHNYM_HPP

#include "CmdBase.hpp"

namespace opentxs
{

class CmdRefreshNym : public CmdBase
{
public:
    EXPORT CmdRefreshNym();
    virtual ~CmdRefreshNym();

    EXPORT int32_t run(std::string server, std::string mynym);

private:
    std::int32_t retrieve_nym(
        const std::string& strNotaryID,
        const std::string& strMyNymID,
        bool& bWasMsgSent,
        bool bForceDownload) const;

protected:
    virtual int32_t runWithOptions();
};

}  // namespace opentxs

#endif  // OPENTXS_CLIENT_CMDREFRESHNYM_HPP
