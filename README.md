opentxs-cli
==============

### Build Instructions

OpenTransactions uses the CMake build system. The basic steps are:

    mkdir build
    cd build
    cmake ..
    make
    make install

This assumes you have [opentxs](https://github.com/Open-Transactions/opentxs)
installed and available on the system.

### Contributing

If you are planning to contribute please contact the devs in #opentransactions @ freenode.net IRC chat.

All development goes in develop branch - please don't submit pull requests to master.

Please do *NOT* use an editor that automatically reformats.

#### CppCheck and clang-format Git hooks

For convenience please enable the git hooks which will trigger cppcheck and
clang-format each time you push or commit. To do so type in the repo directory:

    cd .git/hooks
    ln -s ../../scripts/git_hooks/pre-push
    ln -s ../../scripts/git_hooks/pre-commit

To check your code without pushing the following command can be used:

    git push -n

### Dependencies

[Open Transactions library](https://github.com/Open-Transactions/opentxs)
