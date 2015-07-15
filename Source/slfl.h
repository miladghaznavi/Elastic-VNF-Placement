//
//  slfl.h
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 5/4/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#pragma once

#ifndef __SLFL_H__
#define __SLFL_H__
#endif

#include "algorithm.h"
#include "graph.h"

class SimpleLazyFacilityLocation : public Algorithm {
public:
    static const string NAME;
    
private:
    Demand _d;
    Money  _reassignmentCost;
    Money  _migrationCost;
    size_t _reassignmentNum;
    size_t _migrationNum;
    
    Weight** _shortestPaths;
    
    void   _initShortestPaths();
    Weight _shortestPath     (Node&, Demand&);
    Weight _shortestPath     (Node&, vector<Demand>&);
    
    bool _find(vector<Demand>&, Demand&);
    
    void _clearCostStats();
    
    template<typename T>
    void _copy(T&, T&);
    
    template<typename T>
    void _union(T&, T&);
    
    template<typename T>
    void _clear(T&, SubstrateGraph& network_);
    
    int _outsides(Node&, Demand&);
    int _outsides(Node, vector<Demand>&);
    
    Capacity _residualFacCap (Facility&, vector<Facility>&);
    
    void _removeFootPrint(SubstrateGraph::ArcCapacityMap&, vector<Arc>&);
    void _addFootPrint   (SubstrateGraph::ArcCapacityMap&, vector<Arc>&);

public:
    double minCostFlow_(Node, Demand&, SubstrateGraph::ArcCapacityMap&, SubstrateGraph::ArcWeightMap&, SubstrateGraph::FlowMap&);
    double minCostFlow_(Node, vector<Demand>&, SubstrateGraph::ArcCapacityMap&, SubstrateGraph::ArcWeightMap&, SubstrateGraph::FlowMap&);
    double minCostFlow_(Node, Demand&, SubstrateGraph::FlowMap&);
    double minCostFlow_(Node, vector<Demand>&, SubstrateGraph::FlowMap&);
    
    Money migPot_(Facility, Node, SubstrateGraph::FlowMap&, double, Money&);
    Money insPot_(Node, Money& pAsn, SubstrateGraph::FlowMap&, double, vector<Demand>&, Money&);
    Money emgPot_(Facility, Node, SubstrateGraph::FlowMap&, double, Money&);
    Money rmvPot_(Facility, double, vector<Demand>&, vector<Facility>&, vector<vector<Arc> >&, Money&);

    Money bestAssignment_     (Facility&, Demand, SubstrateGraph::FlowMap&);
    Money bestMigrationPot_   (Facility&, Node&, SubstrateGraph::FlowMap&, double);
    Money bestInstallationPot_(Node&, Money&, vector<Demand>&, SubstrateGraph::FlowMap&, double);
    Money bestEmigrationPot_  (Facility, Node&, SubstrateGraph::FlowMap&, double);
    
public:
    SimpleLazyFacilityLocation(Parameters, SubstrateGraph&, KAryFatTree&);
    virtual bool   arrival  (Demand, double);
    virtual void   departure(Demand, double);
    virtual string name     ();
        
    /* Query Functions AFTER an Arrival or a Departure */
    Money  reassignmentCost();
    size_t reassignmentNum ();
    Money  migrationCost   ();
    size_t migrationNum    ();
};