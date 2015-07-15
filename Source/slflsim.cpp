//
//  slflsim.cpp
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 6/5/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "slflsim.h"

SLFLSim::SLFLSim(string path) : Simulator(path) { }

void SLFLSim::setAlg_() {
    _algorithm = new SimpleLazyFacilityLocation(_params, *_network, *_topology);
}

void SLFLSim::setAlgName_() {
    _algName = SimpleLazyFacilityLocation::NAME;
}
