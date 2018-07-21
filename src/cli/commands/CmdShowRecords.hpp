// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDSHOWRECORDS_HPP
#define OPENTXS_CLIENT_CMDSHOWRECORDS_HPP

#include "CmdBase.hpp"

namespace opentxs
{

class CmdShowRecords : public CmdBase
{
public:
    EXPORT CmdShowRecords();
    virtual ~CmdShowRecords();

    EXPORT int32_t
        run(std::string server, std::string mynym, std::string myacct);

protected:
    virtual int32_t runWithOptions();

private:
    int32_t showRecords(const std::string& server, const std::string& mynym,
                        const std::string& myacct);
};

} // namespace opentxs

#endif // OPENTXS_CLIENT_CMDSHOWRECORDS_HPP
