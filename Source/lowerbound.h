//
//  lowerbound.h
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 7/3/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#pragma once

#include <unordered_map>
#include "algorithm.h"

using std::unordered_map;

class LowerBoundAlg : public Algorithm {
private:
    unordered_map<Identifier, Demand> _demands;
    typedef Dijkstra<SubstrateGraph, SubstrateGraph::EdgeWeightMap> ShortestPath;
    double**    _shortestPaths;
    bool        _computed = false;
    
    bool        _maxPossibleVNFResComputed = false;
    bool        _maxPossibleBandwidthResComputed = false;
    
    Capacity    _maxPossibleVNFResCap;
    Capacity    _maxPossibleBandwidthResCap;
    
    void _findShortestPaths();
    Capacity _maxPossibleVNFRes();
    Capacity _maxPossibleBandwidthRes();
    
public:
    static const string NAME;
    
    LowerBoundAlg(Parameters, SubstrateGraph&, KAryFatTree&);
    
    virtual size_t demandNum           ();
    virtual size_t facilityNum         ();
    virtual Money  installationCost    (Time) override;
    virtual Money  transportationCost  (Time) override;

    /* General functions */
    bool   arrival  (Demand, double);
    void   departure(Demand, double);
    string name     ();
    
    /* Query Functions AFTER an Arrival or a Departure */
    Money  reassignmentCost();
    size_t reassignmentNum ();
    Money  migrationCost   ();
    size_t migrationNum    ();
};