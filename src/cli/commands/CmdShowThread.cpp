// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdShowThread.hpp"

#include <opentxs/opentxs.hpp>

#include <ctime>

#define OT_METHOD "opentxs::CmdShowThread::"

namespace opentxs
{
CmdShowThread::CmdShowThread()
{
    command = "showthread";
    args[0] = "--mynym <nym>";
    args[1] = "--thread <threadID>";
    category = catOtherUsers;
    help = "Display the contents of an activity thread.";
}

std::int32_t CmdShowThread::runWithOptions()
{
    return run(getOption("mynym"), getOption("thread"));
}

std::int32_t CmdShowThread::run(std::string mynym, const std::string& threadID)
{
    if (!checkNym("mynym", mynym)) { return -1; }

    const auto& thread = Opentxs::Client().UI().ActivityThread(
        Identifier::Factory(mynym), Identifier::Factory(threadID));
    const auto first = thread.First();
    LogNormal(OT_METHOD)(__FUNCTION__)(thread.DisplayName()).Flush();

    if (false == first->Valid()) { return 0; }

    auto last = first->Last();
    LogNormal(OT_METHOD)(__FUNCTION__)(": * ")(time(first->Timestamp()))(
        " (StorageBox: ")(storage_box_name(first->Type()))(")")
        .Flush();
    LogNormal(OT_METHOD)(__FUNCTION__)(first->Text()).Flush();

    while (false == last) {
        const auto line = thread.Next();
        last = line->Last();
        LogNormal(OT_METHOD)(__FUNCTION__)(": * ")(time(line->Timestamp()))(
            " (StorageBox: ")(storage_box_name(line->Type()))(")")
            .Flush();
        LogNormal(OT_METHOD)(__FUNCTION__)(line->Text()).Flush();
    }

    LogNormal(OT_METHOD)(__FUNCTION__)(" ").Flush();

    return 0;
}

std::string CmdShowThread::time(
    const std::chrono::system_clock::time_point in) const
{
    const auto converted = std::chrono::system_clock::to_time_t(in);

    return std::ctime(&converted);
}
}  // namespace opentxs
