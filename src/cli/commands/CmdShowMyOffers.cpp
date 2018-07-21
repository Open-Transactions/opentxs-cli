// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CmdShowMyOffers.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

using namespace opentxs;
using namespace std;

CmdShowMyOffers::CmdShowMyOffers()
{
    command = "showmyoffers";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    category = catMarkets;
    help = "Show mynym's offers on a particular server.";
}

CmdShowMyOffers::~CmdShowMyOffers() {}

int32_t CmdShowMyOffers::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"));
}

int32_t CmdShowMyOffers::run(string server, string mynym)
{
    if (!checkServer("server", server)) { return -1; }

    if (!checkNym("mynym", mynym)) { return -1; }

    OTDB::OfferListNym* offerList = loadNymOffers(server, mynym);
    if (nullptr == offerList) {
        otOut << "Error: cannot load offer list.\n";
        return -1;
    }

    int32_t items = offerList->GetOfferDataNymCount();
    if (0 > items) {
        otOut << "Error: cannot load offer list item count.\n";
        return -1;
    }

    if (0 == items) {
        otOut << "The offer list is empty.\n";
        return 0;
    }

    // LOOP THROUGH THE OFFERS and sort them into a map_of_maps, key is:
    // scale-instrumentDefinitionID-currencyID
    // the value for each key is a sub-map, with the key: transaction ID and
    // value: the offer data itself.
    MapOfMaps* map_of_maps = convert_offerlist_to_maps(*offerList);
    if (nullptr == map_of_maps) {
        otOut << "Error: cannot load offer map.\n";
        return -1;
    }

    // (FT) TODO: Fix this ridiculous memory leak. map_of_maps is not
    // cleaned up below this point. (Nor are its member pointers and their
    // contents. unique_ptr is not enough.) I think the only reason Eric
    // let this go was because the program ends anyway after the command
    // fires. Still, needs cleanup.

    // output_nymoffer_data is called for each offer, for this nym, as it
    // iterates through the maps.
    //
    // iterate_nymoffers_maps takes a final parameter extra_vals (not seen
    // here)
    // Other sections in the code which use iterate_nymoffers_maps might pass
    // a value here, or expect one to be returned through the same mechansim.
    if (-1 == iterate_nymoffers_maps(*map_of_maps, output_nymoffer_data)) {
        otOut << "Error: cannot iterate nym offer map.\n";
        return -1;
    }

    return 1;
}
