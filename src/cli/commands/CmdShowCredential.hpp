// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDSHOWCREDENTIAL_HPP
#define OPENTXS_CLIENT_CMDSHOWCREDENTIAL_HPP

#include "CmdBase.hpp"

namespace opentxs
{

class CmdShowCredential : public CmdBase
{
public:
    EXPORT CmdShowCredential();
    virtual ~CmdShowCredential();

    EXPORT int32_t run(std::string mynym, std::string id);

protected:
    virtual int32_t runWithOptions();

private:
    std::string findMaster(const std::string& mynym, const std::string& subID);
    std::string findRevoked(const std::string& mynym, const std::string& subID);
};

} // namespace opentxs

#endif // OPENTXS_CLIENT_CMDSHOWCREDENTIAL_HPP
