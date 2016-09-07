#!/usr/bin/env bash

# The goal is to remove the suppressed checks stepwise and fix the issues
# in the same commit. Use cppcheck --inline-suppr for false positives.
sup_warn="--suppress=nullPointer"
sup_info="--suppress=ConfigurationNotChecked"
sup_perf="--suppress=passedByValue"
sup_tmp="--suppress=unreadVariable \
    --suppress=va_list_usedBeforeStarted:src/core/String.cpp \
    --suppress=unsafeClassCanLeak:include/opentxs/core/OTSettings.hpp \
    --suppress=oppositeInnerCondition:src/ext/OTPayment.cpp \
    --suppress=uninitMemberVar:src/core/OTServerContract.cpp \
    --suppress=uninitMemberVar:src/opentxs/opentxs.cpp \
    --suppress=uselessAssignmentPtrArg:src/client/OTClient.cpp:6579 \
    --suppress=useInitializationList:src/client/ot_utility_ot.cpp:438"
suppress="$sup_warn $sup_info $sup_perf $sup_tmp"
enabled="--enable=warning --enable=information --enable=performance \
         --enable=portability --enable=missingInclude --enable=style"
define="-DEXPORT="
includes="-I./include"
# Exit code '1' is returned if arguments are not valid or if no input
# files are provided. Compare 'cppcheck --help'.
args="-f -q --inline-suppr --error-exitcode=2"
cppcheck $args $define $includes $enabled $suppress "$@"
exit $?
