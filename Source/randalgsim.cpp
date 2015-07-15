//
//  randsim.cpp
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 6/6/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "randalgsim.h"

RandAlgorithmSim::RandAlgorithmSim(string path) : Simulator(path) { }

void RandAlgorithmSim::setAlg_() {
    _algorithm = new RandAlgorithm(_params, *_network, *_topology);
}

void RandAlgorithmSim::setAlgName_() {
    _algName = RandAlgorithm::NAME;
}