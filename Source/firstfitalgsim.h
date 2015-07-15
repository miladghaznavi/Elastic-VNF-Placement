//
//  basicalgsim.h
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 6/5/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#pragma once

#include "simulator.h"
#include "firstfitalg.h"

class FirstFitAlgSim : public Simulator {
public:
    FirstFitAlgSim(string);
protected:
    virtual void setAlg_    ();
    virtual void setAlgName_();
};