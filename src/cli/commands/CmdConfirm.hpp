// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDCONFIRM_HPP
#define OPENTXS_CLIENT_CMDCONFIRM_HPP

#include "CmdBase.hpp"

namespace opentxs
{

class CmdConfirm : public CmdBase
{
public:
    EXPORT CmdConfirm();
    virtual ~CmdConfirm();

    EXPORT int32_t
    run(std::string server,
        std::string mynym,
        std::string myacct,
        std::string hisnym,
        std::string index);

    int32_t confirmInstrument(
        const std::string& server,
        const std::string& mynym,
        const std::string& myacct,
        const std::string& hisnym,
        const std::string& instrument,
        int32_t nIndex,
        std::string* pOptionalOutput = nullptr);

protected:
    int32_t runWithOptions() override;

private:
    int32_t activateContract(
        const std::string& server,
        const std::string& mynym,
        const std::string& contract,
        const std::string& name,
        int32_t accounts);
    int32_t confirmAccounts(
        std::string server,
        std::string mynym,
        std::string myacct,
        std::string contract,
        const std::string& name,
        int32_t accounts);
    int32_t confirmPaymentPlan(
        const std::string& mynym,
        const std::string& myacct,
        const std::string& plan,
        std::string* pOptionalOutput = nullptr);
    int32_t confirmSmartContract(
        const std::string& server,
        const std::string& mynym,
        const std::string& myacct,
        const std::string& hisnym,
        const std::string& contract,
        int32_t nIndex,
        std::string* pOptionalOutput = nullptr);
    int32_t sendToNextParty(
        const std::string& server,
        const std::string& mynym,
        const std::string& hisnym,
        const std::string& contract);
    bool showPartyAccounts(
        const std::string& contract,
        const std::string& name,
        int32_t depth);
    bool showPartyAgents(
        const std::string& contract,
        const std::string& name,
        int32_t depth);
};

}  // namespace opentxs

#endif  // OPENTXS_CLIENT_CMDCONFIRM_HPP
