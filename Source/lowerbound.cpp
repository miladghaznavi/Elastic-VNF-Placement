//
//  lowerbound.cpp
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 7/3/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "lowerbound.h"

const string LowerBoundAlg::NAME = "Lowerbond";

LowerBoundAlg::LowerBoundAlg(Parameters params, SubstrateGraph& network, KAryFatTree& topology) :
    Algorithm(params, network, topology) { }

void LowerBoundAlg::_findShortestPaths() {
    //Init dijkstras
    int hostNum = topology_.hostNum();
    int firstHostId = topology_.id(topology_.firstHost());
    _shortestPaths = new double*[hostNum];
    
    for (auto src = topology_.hostIt(); src != INVALID; ++src) {
        int srcIndex = topology_.id(src) - firstHostId;
        _shortestPaths[srcIndex] = new double[hostNum];
        
        ShortestPath dijkstra(network_, network_._edgeWeight);
        SubstrateGraph::DistMap dist(network_);
        dijkstra.distMap(dist);
        dijkstra.run(src);
        
        for (auto dst = topology_.hostIt(); dst != INVALID; ++dst) {
            int dstIndex = topology_.id(dst) - firstHostId;
            _shortestPaths[srcIndex][dstIndex] = dist[dst];
        }//for
    }//for
    
    _computed = true;
}

Money  LowerBoundAlg::installationCost(Time exactGapToNextEvent) {
    return this->LowerBoundAlg::facilityNum() * costs_.f() * exactGapToNextEvent / params_.timeSlotInSecond;
}

Money LowerBoundAlg::transportationCost  (Time gap) {
    if (!_computed) _findShortestPaths();
    Money re = 0;
    
    for (auto i = _demands.begin(); i != _demands.end(); ++i) {
        Identifier firstHostId = topology_.id(topology_.firstHost());
        Identifier srcId = i->second.source() - firstHostId;
        Identifier trgId = i->second.target() - firstHostId;
        re += _shortestPaths[srcId][trgId];
    }//for

    return re * costs_.g() * gap / params_.timeSlotInSecond;
}

Capacity LowerBoundAlg::_maxPossibleVNFRes() {
    if (!_maxPossibleVNFResComputed) {
        Capacity cap = 0;
        for (auto hostIt = topology_.hostIt(); hostIt != INVALID; ++hostIt)
            cap += network_.residual(hostIt);
        _maxPossibleVNFResCap = params_.vnfMaxCapInTermsOfDemandsNum * cap;
        _maxPossibleVNFResComputed = true;
    }//if
    return _maxPossibleVNFResCap;
}

Capacity LowerBoundAlg::_maxPossibleBandwidthRes() {
    if (!_maxPossibleBandwidthResComputed) {
        Capacity cap = 0;
        for (auto hostIt = topology_.hostIt(); hostIt != INVALID; ++hostIt) {
            Node src = topology_.u(topology_.hostEdge(hostIt));
            Node trg = topology_.v(topology_.hostEdge(hostIt));
            
            Arc a = network_.arc(src, trg);
            cap += network_.residual(a);
        }//for
        _maxPossibleBandwidthResCap = cap;
        _maxPossibleBandwidthResComputed = true;
    }//if
    
    return _maxPossibleBandwidthResCap;
}

bool LowerBoundAlg::arrival(Demand d, double lambda) {
    bool re = true;
    
    if (_demands.size() + 1 > _maxPossibleVNFRes()) {
        re = false;
    }//if
    else {
        Capacity used = 0;
        for (auto i = _demands.begin(); i != _demands.end(); ++i) {
            used += (i->second.source() != i->second.target());
        }//for
        used += (d.source() != d.target());
//        used *= 2;
        if (used > _maxPossibleBandwidthRes())
            re = false;
    }//else
    if (re) {
        _demands.insert(std::make_pair(d.identifier(), d));
    }//if
    return re;
}

void LowerBoundAlg::departure(Demand d, double lambda) {
    _demands.erase(d.identifier());
}

string LowerBoundAlg::name () {
    return LowerBoundAlg::NAME;
}

size_t LowerBoundAlg::demandNum() {
    return this->LowerBoundAlg::_demands.size();
}

size_t LowerBoundAlg::facilityNum() {
    return ceil((double)_demands.size() / (double)params_.vnfMaxCapInTermsOfDemandsNum);
}

/* Query Functions AFTER an Arrival or a Departure */
Money  LowerBoundAlg::reassignmentCost() {
    return NO_COST;
}

size_t LowerBoundAlg::reassignmentNum () {
    return NO_COST;
}

Money  LowerBoundAlg::migrationCost   () {
    return NO_COST;
}

size_t LowerBoundAlg::migrationNum    () {
    return NO_COST;
}
