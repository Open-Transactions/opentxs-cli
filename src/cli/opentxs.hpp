/************************************************************
 *
 *                 OPEN TRANSACTIONS
 *
 *       Financial Cryptography and Digital Cash
 *       Library, Protocol, API, Server, CLI, GUI
 *
 *       -- Anonymous Numbered Accounts.
 *       -- Untraceable Digital Cash.
 *       -- Triple-Signed Receipts.
 *       -- Cheques, Vouchers, Transfers, Inboxes.
 *       -- Basket Currencies, Markets, Payment Plans.
 *       -- Signed, XML, Ricardian-style Contracts.
 *       -- Scripted smart contracts.
 *
 *  EMAIL:
 *  fellowtraveler@opentransactions.org
 *
 *  WEBSITE:
 *  http://www.opentransactions.org/
 *
 *  -----------------------------------------------------
 *
 *   LICENSE:
 *   This Source Code Form is subject to the terms of the
 *   Mozilla Public License, v. 2.0. If a copy of the MPL
 *   was not distributed with this file, You can obtain one
 *   at http://mozilla.org/MPL/2.0/.
 *
 *   DISCLAIMER:
 *   This program is distributed in the hope that it will
 *   be useful, but WITHOUT ANY WARRANTY; without even the
 *   implied warranty of MERCHANTABILITY or FITNESS FOR A
 *   PARTICULAR PURPOSE.  See the Mozilla Public License
 *   for more details.
 *
 ************************************************************/

#ifndef __OPENTXS_HPP__
#define __OPENTXS_HPP__

#include <string>
#include <vector>
#include <deque>

class AnyOption;

namespace opentxs
{

class CmdBase;

class Opentxs
{
public:
    Opentxs();
    virtual ~Opentxs();

    int run(int argc, char* argv[]);

private:
    void loadOptions(AnyOption& opt);
    const char* getOption(
        AnyOption& opt,
        const char* optionName,
        const char* defaultName = nullptr);
    int processCommand(AnyOption& opt);
    int runCommand(CmdBase& cmd);

    std::string& ltrim(std::string& s);
    std::string& rtrim(std::string& s);
    std::string& trim(std::string& s);

    const std::string spaces24 = "                        ";

    int newArgc{0};
    char** newArgv{nullptr};
    bool expectFailure{false};
    std::vector<CmdBase*> cmds_;
    
    struct Command {
        Command(int32_t optArgc, char ** optArgv) {
            for (int32_t i = 0; i < optArgc; ++i) {
                args.emplace_back(optArgv[i]);
            }
        }
        Command(const Command& c) {
            args = c.args;
        }
        std::vector<std::string> args;
    };
    std::deque<Command> history;
};
}  // namespace opentxs
#endif  // __OPENTXS_HPP__
