// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDSENDMESSAGE_HPP
#define OPENTXS_CLIENT_CMDSENDMESSAGE_HPP

#include "CmdBase.hpp"

#include <cstdint>
#include <string>

namespace opentxs
{

class CmdSendMessage : public CmdBase
{
public:
    EXPORT CmdSendMessage();
    EXPORT virtual ~CmdSendMessage() = default;

    EXPORT std::int32_t run(
        std::string server,
        std::string mynym,
        std::string hisnym);

protected:
    std::int32_t runWithOptions() override;

private:
    std::int32_t nym(
        const std::string& server,
        const std::string& mynym,
        std::string& hisnym,
        const std::string& message);

    std::int32_t contact(
        const std::string& mynym,
        const std::string& hisnym,
        const std::string& message);
};
} // namespace opentxs

#endif // OPENTXS_CLIENT_CMDSENDMESSAGE_HPP
