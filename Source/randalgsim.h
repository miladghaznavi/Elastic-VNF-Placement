//
//  randsim.h
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 6/6/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#pragma once

#include "simulator.h"
#include "randalg.h"

class RandAlgorithmSim : public Simulator {
public:
    RandAlgorithmSim(string);
protected:
    virtual void setAlg_    ();
    virtual void setAlgName_();
};