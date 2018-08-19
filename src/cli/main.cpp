// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <opentxs/opentxs.hpp>

#include "opentxs.hpp"

opentxs::Opentxs::PasswordCallback* callback_{nullptr};
opentxs::OTCaller* password_caller_{nullptr};

int main(int argc, char* argv[])
{
    callback_ = new opentxs::Opentxs::PasswordCallback;

    OT_ASSERT(nullptr != callback_)

    password_caller_ = new opentxs::OTCaller;

    OT_ASSERT(nullptr != password_caller_)

    password_caller_->setCallback(callback_);

    OT_ASSERT(password_caller_->isCallbackSet())

    opentxs::OT::Start({}, {}, password_caller_);

    int returnValue{0};
    {
        opentxs::Opentxs opentxsCLI(opentxs::OT::App().StartClient({}, 0));
        returnValue = opentxsCLI.run(argc, argv);
    }

    opentxs::OT::Cleanup();
    delete password_caller_;
    delete callback_;

    return returnValue;
}
