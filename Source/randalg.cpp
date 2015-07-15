//
//  randalg.cpp
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 6/6/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "randalg.h"

using std::cout;
using std::endl;

const string RandAlgorithm::NAME = "Random";

RandAlgorithm::RandAlgorithm(Parameters params, SubstrateGraph& network, KAryFatTree& topology) :
    Algorithm(params, network, topology) { }

bool RandAlgorithm::_assignment(Demand& d, Facility& v, SubstrateGraph::FlowMap& flow) {
    bool re = false;
    vector<unordered_map<Identifier, Facility>::iterator> feasibleFac;
    for (auto it = mapping_.facilities_.begin(); it != mapping_.facilities_.end(); ++it) {
        if (it->second.residualCapacity() > 0) {
            SubstrateGraph::SupplyMap supply(network_, NO_SUPPLY);
            Node nv = network_.nodeFromId(it->second.location());
            setSupply_(nv, d, supply);
            
            // Set min cost flow algorithm parameters
            SubstrateGraph::MinCostFlow mincost(network_);
            mincost.upperMap(network_._arcResidual).costMap(network_._arcWeight).supplyMap(supply);
            
            auto re = mincost.run();
            if (re == mincost.OPTIMAL) {
                feasibleFac.push_back(it);
            }//if
            resetSupply_(nv, d, supply);
        }//if
    }//for
    
    if (feasibleFac.size() > 0) {
        re = true;
        auto randval = rand();
        Identifier index = randval % feasibleFac.size();
        v = feasibleFac[index]->second;
        
        SubstrateGraph::SupplyMap supply(network_, NO_SUPPLY);
        Node nv = network_.nodeFromId(v.location());
        setSupply_(nv, d, supply);
        // Set min cost flow algorithm parameters
        SubstrateGraph::MinCostFlow mincost(network_);
        mincost.upperMap(network_._arcResidual).costMap(network_._arcWeight).supplyMap(supply);
        mincost.run();
        mincost.flowMap(flow);
    }//if
    return re;
}

bool RandAlgorithm::_install(Demand& d, Node& nv, SubstrateGraph::FlowMap& flow) {
    bool re = false;
    vector<Node> feasibleNodes;
    for (auto n = topology_.hostIt(); n != INVALID; ++n) {
        if (network_.residual(n) > 0){
            SubstrateGraph::SupplyMap supply(network_, NO_SUPPLY);
            setSupply_(n, d, supply);
            
            // Set min cost flow algorithm parameters
            SubstrateGraph::MinCostFlow mincost(network_);
            mincost.upperMap(network_._arcResidual).costMap(network_._arcWeight).supplyMap(supply);
            
            auto re = mincost.run();
            if (re == mincost.OPTIMAL) {
                feasibleNodes.push_back(n);
            }//if
            resetSupply_(n, d, supply);
        }//if
    }//for
    
    if (feasibleNodes.size() > 0) {
        Identifier index = rand() % feasibleNodes.size();
        nv = feasibleNodes[index];
        re = true;
        
        SubstrateGraph::SupplyMap supply(network_, NO_SUPPLY);
        setSupply_(nv, d, supply);
        // Set min cost flow algorithm parameters
        SubstrateGraph::MinCostFlow mincost(network_);
        mincost.upperMap(network_._arcResidual).costMap(network_._arcWeight).supplyMap(supply);
        mincost.run();
        mincost.flowMap(flow);
    }//if
    return re;
}

bool RandAlgorithm::arrival(Demand d, double lambda) {
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

void RandAlgorithm::departure(Demand d, double lambda) {
    d = mapping_.demand(d.identifier());
    Facility v = mapping_.facility(d);
    
    // Remove demand and its assigned resources
    mapping_.deassign(d, network_);
    mapping_.remove(d);
    
    if (v.allocated() - 1 == 0) {
        remove_(v);
    }//if
}

string RandAlgorithm::name() {
    return RandAlgorithm::NAME;
}

Money  RandAlgorithm::reassignmentCost() {
    return NO_COST;
}

size_t RandAlgorithm::reassignmentNum() {
    return 0;
}

Money  RandAlgorithm::migrationCost() {
    return NO_COST;
}

size_t RandAlgorithm::migrationNum() {
    return 0;
}
