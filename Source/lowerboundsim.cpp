//
//  lowerboundsim.cpp
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 7/4/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "lowerboundsim.h"
#include "lowerbound.h"

LowerBoundAlgSim::LowerBoundAlgSim(string path) : Simulator(path) { }

void LowerBoundAlgSim::setAlg_() {
    _algorithm = new LowerBoundAlg(_params, *_network, *_topology);
}

void LowerBoundAlgSim::setAlgName_() {
    _algName = LowerBoundAlg::NAME;
}