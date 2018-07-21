// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDBASEACCEPT_HPP
#define OPENTXS_CLIENT_CMDBASEACCEPT_HPP

#include "CmdBase.hpp"

namespace opentxs
{

class CmdBaseAccept : public CmdBase
{
protected:
    //enum ItemType { typeBoth = 0, typeTransfers = 1, typeReceipts = 2 };

public:
    EXPORT int32_t acceptFromInbox(
        const std::string& myacct,
        const std::string& indices,
        const std::int32_t itemType) const;
    EXPORT int32_t acceptFromPaymentbox(
        const std::string& transport_notary,
        const std::string& myacct,
        const std::string& indices,
        const std::string& paymentType,
        std::string* pOptionalOutput = nullptr) const;
};

}  // namespace opentxs

#endif  // OPENTXS_CLIENT_CMDBASEACCEPT_HPP
