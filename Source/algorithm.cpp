//
//  algorithm.cpp
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 5/4/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "algorithm.h"
#include <stdexcept>
#include <sstream>

Algorithm::Algorithm(Parameters& params, SubstrateGraph& network, KAryFatTree& topology) :
            costs_(params, network, topology), mapping_() {
    params_ = params;
    network.copyTo(this->network_);
    topology.copyTo(this->topology_);
    demandsSeen_ = 0;
    facilitiesSeen_ = 0;
}

double Algorithm::setSupply_(Node& n, Demand& d, SubstrateGraph::SupplyMap& supply) {
    // Set supplies of demands
    Supply supplyAtFacility = 0;
    Node src = network_.nodeFromId(d.source());
    Node trg = network_.nodeFromId(d.target());
    if (src != n) {
        supply[src] += DEMAND_SOURCE_SUPPLY;
        supplyAtFacility += abs(DEMAND_SOURCE_SUPPLY);
    }//if
    if (trg != n) {
        supply[trg] += DEMAND_TARGET_SUPPLY;
        supplyAtFacility += abs(DEMAND_TARGET_SUPPLY);
    }//if
    
    //if all of them are in a same node
    if (supplyAtFacility == 0) {
        return NO_COST;
    }//if
    
    // Set supply of node 'n'
    supply[n] = supplyAtFacility;
    
    return supplyAtFacility;
}

double Algorithm::resetSupply_(Node& n, Demand& d, SubstrateGraph::SupplyMap& supply) {
    // Set supplies of demands
    Supply supplyAtFacility = 0;
    Node src = network_.nodeFromId(d.source());
    Node trg = network_.nodeFromId(d.target());
    if (src != n) {
        supply[src] -= DEMAND_SOURCE_SUPPLY;
        supplyAtFacility -= abs(DEMAND_SOURCE_SUPPLY);
    }//if
    if (trg != n) {
        supply[trg] -= DEMAND_TARGET_SUPPLY;
        supplyAtFacility -= abs(DEMAND_TARGET_SUPPLY);
    }//if
    
    //if all of them are in a same node
    if (supplyAtFacility == 0) {
        return NO_COST;
    }//if
    
    // Set supply of node 'n'
    supply[n] = supplyAtFacility;
    
    return supplyAtFacility;
}


Facility Algorithm::install_(Node nv) {
    if (network_.residual(nv) == 0) {
        std::stringstream ss;
        ss << "There is not no capacity in node " << network_.id(nv) << " to install a new facility!";
        throw std::logic_error(ss.str());
    }//if
    
    Facility v(network_.id(nv), params_.vnfMaxCapInTermsOfDemandsNum);
    v.identifier(facilitiesSeen_++);
    mapping_.install(v, network_);
    return v;
}

void Algorithm::remove_(Facility v) {
    Node nv = network_.nodeFromId(v.location());
    if (network_.allocated(nv) == 0) {
        std::stringstream ss;
        ss << "No facility exists in node " << network_.id(nv) << " to be removed!";
        throw std::logic_error(ss.str());
    }//if
    mapping_.remove(v, network_);
}

size_t Algorithm::demandNum() {
    return mapping_.demandNum();
}

Money Algorithm::installationCost(Time exactGapToNextEvent) {
    return mapping_.facilityNum() * costs_.f() * exactGapToNextEvent / params_.timeSlotInSecond;
}

size_t Algorithm::facilityNum() {
    return mapping_.facilityNum();
}

Money Algorithm::transportationCost(Time exactGapToNextEvent) {
    Money re = 0;
    for (ArcIt a(network_); a != INVALID; ++a) {
        re += costs_.g() * network_.allocated(a) * network_.weight(a) * exactGapToNextEvent / params_.timeSlotInSecond;
    }//for
    // We devide it by two because we compute the cost of each link twice (Two arcs represents a link)
    return re / 2;
}

Money Algorithm::transportationCostOf(Demand d, Time gap){
    Money re = 0;
    vector<Arc> arcs;
    mapping_.flow(d, arcs);
    
    for (size_t i = 0; i < arcs.size(); i++)
        re += costs_.g() * network_.weight(arcs[i]) * gap / params_.timeSlotInSecond;
    
    return re;
}

Money Algorithm::transportationCostOf(Demand d) {
    return transportationCostOf(d, params_.timeSlotInSecond);
}

Money Algorithm::transportationCostOf(vector<Demand>& D, Time gap) {
    Money re = 0;
    for (size_t i = 0; i < D.size(); ++i) {
        re += transportationCostOf(D[i], gap);
    }//for
    
    return re;
}

Money Algorithm::transportationCostOf(vector<Demand>& D) {
    return transportationCostOf(D, params_.timeSlotInSecond);
}

