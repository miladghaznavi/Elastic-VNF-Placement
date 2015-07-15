//
//  slflsim.h
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 6/5/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#pragma once

#include "simulator.h"
#include "slfl.h"

class SLFLSim : public Simulator {
public:
    SLFLSim(string);
protected:
    virtual void setAlg_    ();
    virtual void setAlgName_();
};