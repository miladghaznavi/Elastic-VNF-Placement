//
//  lowerboundsim.h
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 7/4/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#pragma once

#include "simulator.h"
#include "lowerbound.h"

class LowerBoundAlgSim : public Simulator {
public:
    LowerBoundAlgSim(string);
protected:
    virtual void setAlg_    ();
    virtual void setAlgName_();
};