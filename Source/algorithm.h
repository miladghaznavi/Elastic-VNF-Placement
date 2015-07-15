//
//  algorithm.h
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 5/4/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#pragma once

#include "Mapping.h"
#include "fattree.h"
#include "substrategraph.h"
#include "network.h"
#include "cost.h"
#include <vector>

#define DEMAND_SOURCE_SUPPLY -1
#define DEMAND_TARGET_SUPPLY -1

class Algorithm {
#ifdef _REPORT_H_
friend class Report;
#endif

protected:
    SubstrateGraph network_;
    KAryFatTree    topology_;
    Mapping        mapping_;
    Cost           costs_;
    Parameters     params_;
    
    int demandsSeen_;
    int facilitiesSeen_;
    
    double setSupply_  (Node&, Demand&, SubstrateGraph::SupplyMap&);
    double resetSupply_(Node&, Demand&, SubstrateGraph::SupplyMap&);
    
    Facility install_(Node);
    void     remove_ (Facility);
    
public:
    Algorithm(Parameters&, SubstrateGraph&, KAryFatTree&);
    
    /* Query Functions AFTER an Arrival or a Departure */
    virtual size_t demandNum           ();
    virtual size_t facilityNum         ();
    virtual Money  installationCost    (Time);
    virtual Money  transportationCost  (Time);
    virtual Money  transportationCostOf(Demand, Time);
    virtual Money  transportationCostOf(Demand);
    virtual Money  transportationCostOf(vector<Demand>&, Time);
    virtual Money  transportationCostOf(vector<Demand>&);
    
    /* General functions */
    virtual bool   arrival  (Demand, double) = 0;
    virtual void   departure(Demand, double) = 0;
    virtual string name     ()               = 0;
    
    /* Query Functions AFTER an Arrival or a Departure */
    virtual Money  reassignmentCost()     = 0;
    virtual size_t reassignmentNum ()     = 0;
    virtual Money  migrationCost   ()     = 0;
    virtual size_t migrationNum    ()     = 0;
};
