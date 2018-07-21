// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CLIENT_CMDEXPORTCASH_HPP
#define OPENTXS_CLIENT_CMDEXPORTCASH_HPP

#include "CmdBaseInstrument.hpp"

namespace opentxs
{

class CmdExportCash : public CmdBaseInstrument
{
public:
    EXPORT CmdExportCash();
    virtual ~CmdExportCash();

    EXPORT int32_t
    run(std::string server,
        std::string mynym,
        std::string mypurse,
        std::string hisnym,
        std::string indices,
        std::string password);

    EXPORT std::string exportCash(
        const std::string& server,
        const std::string& mynym,
        const std::string& assetType,
        std::string& hisnym,
        const std::string& indices,
        bool hasPassword,
        std::string& retainedCopy) const;

private:
    std::string load_or_retrieve_encrypt_key(
        const std::string& NOTARY_ID,
        const std::string& NYM_ID,
        const std::string& TARGET_NYM_ID) const;

    std::string load_public_encryption_key(const std::string& NYM_ID) const;

protected:
    int32_t runWithOptions() override;
};

}  // namespace opentxs

#endif  // OPENTXS_CLIENT_CMDEXPORTCASH_HPP
