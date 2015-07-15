//
//  basicalgsim.cpp
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 6/5/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "firstfitalgsim.h"

FirstFitAlgSim::FirstFitAlgSim(string path) : Simulator(path) { }

void FirstFitAlgSim::setAlg_() {
    _algorithm = new FirstFitAlg(_params, *_network, *_topology);
}

void FirstFitAlgSim::setAlgName_() {
    _algName = FirstFitAlg::NAME;
}
