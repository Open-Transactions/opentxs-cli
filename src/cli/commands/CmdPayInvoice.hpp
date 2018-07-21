// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDPAYINVOICE_HPP
#define OPENTXS_CLIENT_CMDPAYINVOICE_HPP

#include "CmdBase.hpp"

namespace opentxs
{

class CmdPayInvoice : public CmdBase
{
public:
    EXPORT CmdPayInvoice();
    virtual ~CmdPayInvoice();

    EXPORT int32_t processPayment(
        const std::string& transport_notary,
        const std::string& myacct,
        const std::string& paymentType,
        const std::string& inbox,
        const int32_t index,
        std::string * pOptionalOutput=nullptr);

    EXPORT int32_t run(std::string server,
                       std::string myacct,
                       std::string index);

protected:
    int32_t runWithOptions() override;
};

}  // namespace opentxs

#endif  // OPENTXS_CLIENT_CMDPAYINVOICE_HPP
