//
//  basicalg.cpp
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 6/4/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "firstfitalg.h"

using std::cout;
using std::endl;

const string FirstFitAlg::NAME = "FirstFit";

FirstFitAlg::FirstFitAlg(Parameters params, SubstrateGraph& network, KAryFatTree& topology) :
Algorithm(params, network, topology) { }

bool FirstFitAlg::_assignment(Demand& d, Facility& v, SubstrateGraph::FlowMap& flow) {
    bool found = false;
    for (auto it = mapping_.facilities_.begin(); it != mapping_.facilities_.end() && !found; ++it) {
        if (it->second.residualCapacity() > 0) {
            SubstrateGraph::SupplyMap supply(network_, NO_SUPPLY);
            Node nv = network_.nodeFromId(it->second.location());
            
            // Set min cost flow algorithm parameters
            setSupply_(nv, d, supply);
            SubstrateGraph::MinCostFlow mincost(network_);
            mincost.upperMap(network_._arcResidual).costMap(network_._arcWeight).supplyMap(supply);
            
            auto re = mincost.run();
            if (re == mincost.OPTIMAL) {
                found = true;
                mincost.flowMap(flow);
                v = it->second;
            }//if
            resetSupply_(nv, d, supply);
        }//if
    }//for
    return found;
}

bool FirstFitAlg::_install(Demand& d, Node& nv, SubstrateGraph::FlowMap& flow) {
    bool found = false;
    for (auto n = topology_.hostIt(); n != INVALID && !found; ++n) {
        if (network_.residual(n) > 0){
            SubstrateGraph::SupplyMap supply(network_, NO_SUPPLY);
            setSupply_(n, d, supply);
            
            // Set min cost flow algorithm parameters
            SubstrateGraph::MinCostFlow mincost(network_);
            mincost.upperMap(network_._arcResidual).costMap(network_._arcWeight).supplyMap(supply);
            
            auto re = mincost.run();
            if (re == mincost.OPTIMAL) {
                found = true;
                nv = n;
                mincost.flowMap(flow);
            }//if
            resetSupply_(n, d, supply);
        }//if
    }//for
    return found;
}

bool FirstFitAlg::arrival  (Demand d, double lambda) {
    bool re = true;
    mapping_.addDemand(d);
    
    cout << "\tChecking assignment: ";
    // Best current assignment
    Facility vAsn;
    Node nIns;
    SubstrateGraph::FlowMap RAsn(network_, NO_FLOW);
    SubstrateGraph::FlowMap RIns(network_, NO_FLOW);
    if (_assignment(d, vAsn, RAsn)) {
        cout << "\tAssign to facility " << vAsn.identifier() << " in " << vAsn.location() << endl;
        mapping_.assign(vAsn, d, network_, RAsn);
    }//if
    else if (_install(d, nIns, RIns)) {
        cout << "\tInstall a facility at " << network_.id(nIns) << endl;
        Facility vIns = install_(nIns);
        cout << "\tAssign " << d.identifier() << "(" << d.source() << "," << d.target()
        << ") to facility " << vIns.identifier() << " at " << network_.id(nIns) << endl;
        mapping_.assign(vIns, d, network_, RIns);
    }//else if
    else {
        cout << "\tRejected!" << endl;
        mapping_.remove(d);
        re = false;
    }//else
    
    return re;
}

void FirstFitAlg::departure(Demand d, double lambda) {
    d = mapping_.demand(d.identifier());
    Facility v = mapping_.facility(d);
    
    // Remove demand and its assigned resources
    mapping_.deassign(d, network_);
    mapping_.remove(d);
    
    if (v.allocated() - 1 == 0) {
        remove_(v);
    }//if
}

string FirstFitAlg::name() {
    return FirstFitAlg::NAME;
}

Money  FirstFitAlg::reassignmentCost() {
    return NO_COST;
}

size_t FirstFitAlg::reassignmentNum() {
    return NO_COST;
}

Money  FirstFitAlg::migrationCost() {
    return NO_COST;
}

size_t FirstFitAlg::migrationNum() {
    return NO_COST;
}
