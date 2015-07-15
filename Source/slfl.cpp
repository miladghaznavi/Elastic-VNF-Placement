//
//  slfl.cpp
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 5/4/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "slfl.h"
#include <math.h>

using std::cout;
using std::endl;

const string SimpleLazyFacilityLocation::NAME = "SLFL";

SimpleLazyFacilityLocation::SimpleLazyFacilityLocation(Parameters params, SubstrateGraph& network, KAryFatTree& topology) :
    Algorithm(params, network, topology),
    _d(0,0) {
        _clearCostStats();
        _initShortestPaths();
}

void SimpleLazyFacilityLocation::_initShortestPaths() {
    int N = topology_.nodeNum();
    _shortestPaths = new Weight*[N];
    
    for (NodeIt src(network_); src != INVALID; ++src) {
        int srcIndex = network_.id(src);
        _shortestPaths[srcIndex] = new Weight[N];
        
        Cost::ShortestPath dijkstra(network_, network_._edgeWeight);
        SubstrateGraph::DistMap dist(network_);
        dijkstra.distMap(dist);
        dijkstra.run(src);
        
        for (NodeIt dst(network_); dst != INVALID; ++dst) {
            int dstIndex = network_.id(dst);
            _shortestPaths[srcIndex][dstIndex] = dist[dst];
        }//for
    }//for
}

Weight SimpleLazyFacilityLocation::_shortestPath(Node& n, Demand& d) {
    Weight re = 0;Identifier nId = network_.id(n);
    re += _shortestPaths[nId][d.source()];
    re += _shortestPaths[nId][d.target()];
    return re;
}

Weight SimpleLazyFacilityLocation::_shortestPath(Node& n, vector<Demand>& D) {
    Weight re = 0;
    for (size_t i = 0; i < D.size(); i++)
        re += _shortestPath(n, D[i]);
    return re;
}

double SimpleLazyFacilityLocation::minCostFlow_(Node n, Demand& d, SubstrateGraph::ArcCapacityMap& arcResidual, SubstrateGraph::ArcWeightMap& arcWeight, SubstrateGraph::FlowMap& flows) {
    // Set supplies of demands
    SubstrateGraph::SupplyMap supply(network_, NO_SUPPLY);
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
    
    // Set min cost flow algorithm parameters
    SubstrateGraph::MinCostFlow mincost(network_);
    mincost.upperMap(arcResidual).costMap(arcWeight).supplyMap(supply);
    
    double cost = INFINITY;
    auto re = mincost.run();
    if (re == mincost.OPTIMAL) {
        mincost.flowMap(flows);
        cost = mincost.totalCost();
    }//if
    
    return cost;
}

double SimpleLazyFacilityLocation::minCostFlow_(Node n, vector<Demand>& D, SubstrateGraph::ArcCapacityMap& arcResidual, SubstrateGraph::ArcWeightMap& arcWeight, SubstrateGraph::FlowMap& flows) {
    // Set supplies of demands
    SubstrateGraph::SupplyMap supply(network_, NO_SUPPLY);
    Supply supplyAtFacility = 0;
    for (size_t i = 0; i < D.size(); i++) {
        Node src = network_.nodeFromId(D[i].source());
        Node trg = network_.nodeFromId(D[i].target());
        if (src != n) {
            supply[src] += DEMAND_SOURCE_SUPPLY;
            supplyAtFacility += abs(DEMAND_SOURCE_SUPPLY);
        }//if
        if (trg != n) {
            supply[trg] += DEMAND_TARGET_SUPPLY;
            supplyAtFacility += abs(DEMAND_TARGET_SUPPLY);
        }//if
    }//for
    
    //if all of them are in a same node
    if (supplyAtFacility == 0) {
        return NO_COST;
    }//if
    
    // Set supply of node 'n'
    supply[n] = supplyAtFacility;
    
    // Set min cost flow algorithm parameters
    SubstrateGraph::MinCostFlow mincost(network_);
    mincost.upperMap(arcResidual).costMap(arcWeight).supplyMap(supply);
    
    double cost = INFINITY;
    auto re = mincost.run();
    if (re == mincost.OPTIMAL) {
        mincost.flowMap(flows);
        cost = mincost.totalCost();
    }//if
    
    return cost;
}

double SimpleLazyFacilityLocation::minCostFlow_(Node n, vector<Demand>& D, SubstrateGraph::FlowMap& flows) {
    return minCostFlow_(n, D, network_._arcResidual, network_._arcWeight, flows);
}

double SimpleLazyFacilityLocation::minCostFlow_(Node n, Demand& d, SubstrateGraph::FlowMap& flows) {
    return minCostFlow_(n, d, network_._arcResidual, network_._arcWeight, flows);
}

bool SimpleLazyFacilityLocation::_find(vector<Demand>& DStar, Demand& findIt) {
    bool found = false;
    for (size_t i = 0; i < DStar.size() && !found; i++) {
        if (DStar[i] == findIt)
            found = true;
    }//for
    return found;
}

void SimpleLazyFacilityLocation::_clearCostStats() {
    _reassignmentCost =
    _migrationCost    =
    _reassignmentNum  =
    _migrationNum     = NO_COST;
}

template<typename T>
void SimpleLazyFacilityLocation::_copy(T& src, T& dst) {
    for (ArcIt a(network_); a != INVALID; ++a)
        dst[a] = src[a];
}

template<typename T>
void SimpleLazyFacilityLocation::_union(T& src, T& dst) {
    for (ArcIt a(network_); a != INVALID; ++a)
        dst[a] += src[a];
}

template<typename T>
void SimpleLazyFacilityLocation::_clear(T& list, SubstrateGraph& network_) {
    for (ArcIt a(network_); a != INVALID; ++a) {
        list[a] = NO_CAPACITY;
    }//for
}

void SimpleLazyFacilityLocation::_removeFootPrint(SubstrateGraph::ArcCapacityMap& residual, vector<Arc>& paths) {
    for (size_t i = 0; i < paths.size(); i++) {
        Network::removeFootPrint(network_, residual, paths[i], BANDWIDTH_UNIT);
    }//for
}

int SimpleLazyFacilityLocation::_outsides(Node& n, Demand& d) {
    int re = 0;
    Identifier nId = network_.id(n);
    if (d.source() != nId) re ++;
    if (d.target() != nId) re ++;
    return re;
}

int SimpleLazyFacilityLocation::_outsides(Node n, vector<Demand>& D) {
    int re = 0;
    for (size_t i = 0; i < D.size(); ++i) {
        re += _outsides(n, D[i]);
    }//for
    return re;
}

Capacity SimpleLazyFacilityLocation::_residualFacCap(Facility& v, vector<Facility>& vect) {
    Capacity re = v.residualCapacity();
    
    for (size_t i = 0; i < vect.size(); ++i)
        if (v.identifier() == vect[i].identifier())
            re --;
    
    return re;
}

void SimpleLazyFacilityLocation::_addFootPrint(SubstrateGraph::ArcCapacityMap& residual, vector<Arc>& paths) {
    for (size_t i = 0; i < paths.size(); i++) {
        Network::addFootPrint(network_, residual, paths[i], BANDWIDTH_UNIT);
    }//for
}

Money SimpleLazyFacilityLocation::migPot_(Facility v, Node n, SubstrateGraph::FlowMap& RStar, double lambda, Money& reportMigCost) {
    if (network_.residual(n) == 0 || v.residualCapacity() == 0) {
        return -INFINITY;
    }//if
    
    vector<Demand> Dv;
    mapping_.demands(v, Dv);
    
    // The current cost = Transportation cost of Dv
    Money C = transportationCostOf(Dv);
    
    // Find k
    Node nv = network_.nodeFromId(v.location());
    reportMigCost = costs_.k(nv, n, network_, topology_);
    
    SubstrateGraph::ArcCapacityMap residual(network_);
    _copy(network_._arcResidual, residual);
    vector<Arc> paths;
    mapping_.flow(v, paths);
    
    // Artificially omit network footprint of Dv;
    _removeFootPrint(residual, paths);
    
    //  Check if the host link has sufficient residual cap
    // to accept traffic
    // In most cases, host links are more utilized
    Edge nEdge = topology_.hostEdge(n);
    Arc  nArc  = network_.arc(network_.u(nEdge), network_.v(nEdge));
    Dv.push_back(_d);
    if (residual[nArc] < (_outsides(n, Dv) * BANDWIDTH_UNIT)) {
        return -INFINITY;
    }//if
    // The transportation cost of Dv union {d} after possible migration
    Money CStar = costs_.g() * minCostFlow_(n, Dv, residual, network_._arcWeight, RStar) + lambda * reportMigCost;
    Dv.pop_back();
    
    //Back network footprint
    // We don't need to :)
    
    return (std::isinf(CStar)) ? - INFINITY : C - CStar;
}

Money SimpleLazyFacilityLocation::insPot_(Node n, Money& pAsn, SubstrateGraph::FlowMap& RStar, double lambda, vector<Demand>& DStar, Money& reportReasnCost) {
    if (network_.residual(n) == 0) {
        return -INFINITY;
    }//if
    
    Money eStar = - costs_.g() * minCostFlow_(n, _d, RStar);
    if (!isinf(pAsn)) {
        eStar += pAsn;
    }//if
    
    SubstrateGraph::FlowMap Routes(network_);
    SubstrateGraph::FlowMap R(network_, NO_FLOW);
    
    SubstrateGraph::ArcCapacityMap residual(network_);
    _copy(network_._arcResidual, residual);
    
    vector<Arc> paths;
    for (int j = 0; j < params_.vnfMaxCapInTermsOfDemandsNum - 1; j++) {
        _clear(Routes, network_);
        Money e = 0;
        Money reasnCostStar = 0;
        Demand dStar(0,0);
        for (auto it = mapping_.demands_.begin(); it != mapping_.demands_.end(); it++) {
            Demand i = it->second;
            if (_find(DStar, i) || i == _d) continue;
            
            Facility iFac = mapping_.facility(i);
            
            DStar.push_back(i);
            Money C = transportationCostOf(DStar);
            
            if (!isinf(pAsn))
                C += pAsn;
            
            // Reassignment Cost
            Money reasnCost = costs_.h(network_.nodeFromId(iFac.location()), n, network_, topology_);
            
            paths.clear();
            mapping_.flow(DStar, paths);
            _removeFootPrint(residual, paths);
            
            //  Check if the host link has residual cap
            // In most cases, host links are more utilized
            Edge nEdge = topology_.hostEdge(n);
            Arc  nArc  = network_.arc(network_.u(nEdge), network_.v(nEdge));            
            // Push _d and find the minimum cost flow
            DStar.push_back(_d);
            Money CStar = INFINITY;
            _clear(R, network_);
            if (residual[nArc] >= (_outsides(n, DStar) * BANDWIDTH_UNIT) &&
                e < (C - costs_.g() * _shortestPath(n, DStar))) {
                CStar = costs_.g() * minCostFlow_(n, DStar, residual, network_._arcWeight, R) + lambda * reasnCost;
            }//if
            // Pop _d
            DStar.pop_back();
            
            // Back network footprint and remove i from the vector
            _addFootPrint(residual, paths);
            DStar.pop_back();
            
            if (e < C - CStar) {
                reasnCostStar = reasnCost;
                e = C - CStar;
                dStar = i;
                _copy(R, Routes);
            }//if
        }//for
        
        if (e > 0 && e > eStar) {
            DStar.push_back(dStar);
            reportReasnCost += reasnCostStar;
            eStar = e;
            _copy(RStar, Routes);
        }//if
        else {
            break;
        }//else
    }//for
    
    eStar = (isinf(pAsn)) ? (eStar - costs_.f()) : (eStar - costs_.f() - pAsn);
    
    return eStar;
}

Money SimpleLazyFacilityLocation::emgPot_(Facility v, Node n, SubstrateGraph::FlowMap& RStar, double lambda, Money& reportMigCost) {
    if (network_.residual(n) == 0) {
        return -INFINITY;
    }//if
    
    vector<Demand> Dv;
    mapping_.demands(v, Dv);
    
    // The current cost = Transportation cost of Dv + Cost of first action (p)
    Money C = transportationCostOf(Dv);
    
    // Find k
    Node nv = network_.nodeFromId(v.location());
    reportMigCost = costs_.k(nv, n, network_, topology_);
    
    SubstrateGraph::ArcCapacityMap residual(network_);
    _copy(network_._arcResidual, residual);
    vector<Arc> paths;
    mapping_.flow(v, paths);
    
    // Artificially omit network footprint of Dv;
    _removeFootPrint(residual, paths);
    
    //  Check if the host link has sufficient residual cap
    // to accept traffic
    // In most cases, host links are more utilized
    Edge nEdge = topology_.hostEdge(n);
    Arc  nArc  = network_.arc(network_.u(nEdge), network_.v(nEdge));
    if (residual[nArc] < (_outsides(n, Dv) * BANDWIDTH_UNIT)) {
        return -INFINITY;
    }//if
    
    //The transportation cost of Dv after possible migration
    Money CStar = costs_.g() * minCostFlow_(n, Dv, residual, network_._arcWeight, RStar) + lambda * reportMigCost;
    
    //Back network footprint
    // We don't need to :)
    
    return (std::isinf(CStar)) ? - INFINITY : C - CStar;
}

Money SimpleLazyFacilityLocation::rmvPot_(Facility v, double lambda, vector<Demand>& Dre, vector<Facility>& Vre, vector<vector<Arc> >& RStar, Money& reportReasnCost) {
    vector<Demand> Dv;
    mapping_.demands(v, Dv);
    Node nv = network_.nodeFromId(v.location());
    
    Money eStar = costs_.f();
    Money C = transportationCostOf(Dv);
    
    reportReasnCost = 0;
    Money CStar = 0;
    
    SubstrateGraph::ArcCapacityMap residual(network_);
    _copy(network_._arcResidual, residual);
    
    //Artificially omit network footprint of Dv
    vector<Arc> paths;
    mapping_.flow(Dv, paths);
    _removeFootPrint(residual, paths);
    
    SubstrateGraph::FlowMap flows  (network_, NO_FLOW);
    SubstrateGraph::FlowMap tmpFlow(network_, NO_FLOW);
    vector<Arc> arcs;
    
    for (Demand d: Dv) {
        Facility bestV;
        Money bestVCost = INFINITY;
        double g = costs_.g();
        
        //Finding the best facility to reassign
        for (auto facIt = mapping_.facilities_.begin(); facIt != mapping_.facilities_.end(); facIt++) {
            if (facIt->second.identifier() == v.identifier()) continue;
            if (_residualFacCap(facIt->second, Vre) <= 0)     continue;
            
            Node facNode = network_.nodeFromId(facIt->second.location());
            
            //  Check if the host link has residual cap
            // In most cases, host links are more utilized
            Edge facEdge = topology_.hostEdge(facNode);
            Arc  facArc  = network_.arc(network_.u(facEdge), network_.v(facEdge));
            
            if (residual[facArc] < (_outsides(facNode, d) * BANDWIDTH_UNIT)) {
                continue;
            }//if
            
            _clear(tmpFlow, network_);
            Money cost = g * minCostFlow_(facNode, d, residual, network_._arcWeight, tmpFlow);
            if (cost < bestVCost) {
                bestV = facIt->second;
                bestVCost = cost;
                _copy(tmpFlow, flows);
            }//if
        }//for
        
        Money reasnCost = (isinf(bestVCost)) ? INFINITY : costs_.h(nv, network_.nodeFromId(bestV.location()), network_, topology_);
        reportReasnCost += reasnCost;
        CStar += bestVCost + lambda * reasnCost;
        
        if (std::isinf(CStar)) {
            //Roll back all changes
            eStar = 0;
            Dre.clear();
            Vre.clear();
            RStar.clear();
            break;
        }//if
        Dre.push_back(d);
        Vre.push_back(bestV);
        
        arcs.clear();
        mapping_.findFlowPath_(bestV, d, network_, flows, arcs);
        RStar.push_back(arcs);
        _addFootPrint(residual, arcs);
        
        _clear(flows, network_);
    }//for
    
    eStar += C - CStar;
    
    return eStar;
}

Money SimpleLazyFacilityLocation::bestAssignment_(Facility& bestFac, Demand d, SubstrateGraph::FlowMap& RStar) {
    vector<Demand> D;
    D.push_back(d);
    SubstrateGraph::FlowMap flows(network_);
    
    Money bestCost = INFINITY;
    
    for (auto it = mapping_.facilities_.begin(); it != mapping_.facilities_.end(); ++it) {
        // Check capacity of vnf
        if (it->second.residualCapacity() == 0) continue;
        
        //  Check if the host link has residual cap
        // In most cases, host links are more utilized
        Node host = network_.nodeFromId(it->second.location());
        Edge hostEdge = topology_.hostEdge(host);
        Arc  hostArc  = network_.arc(network_.u(hostEdge), network_.v(hostEdge));
        if (network_.residual(hostArc) == 0) continue;
        
        // Find min cost flow
        _clear(flows, network_);
        Money C = costs_.g() * minCostFlow_(host, D, flows);
        if (C < bestCost) {
            bestCost = C;
            bestFac = it->second;
            _copy(flows, RStar);
        }//if
    }//for
    
    return bestCost;
}

Money SimpleLazyFacilityLocation::bestMigrationPot_(Facility& vMig, Node& nMig, SubstrateGraph::FlowMap& RStar, double lambda) {
    SubstrateGraph::FlowMap flows(network_, NO_FLOW);
    
    Money bestPot = -INFINITY;
    size_t facCount = mapping_.facilities_.size();
    for (auto n = topology_.hostIt(); n != INVALID && facCount > 0; ++n) {
        if (network_.residual(n) == 0) continue;
        
        for (auto v = mapping_.facilities_.begin(); v != mapping_.facilities_.end(); ++v) {
            // If facility does not have capacity for a new demand,
            //there is no point to migrate it
            if (v->second.residualCapacity() == 0) continue;
            
            // There is no point to migrate facility to its current location
            if (v->second.location() == network_.id(n)) continue;
            
            Money reportMigCost = NO_COST;
            _clear(flows, network_);
            Money pot = migPot_(v->second, n, flows, lambda, reportMigCost);
            if (pot > bestPot) {
                bestPot = pot;
                _copy(flows, RStar);
                vMig = v->second;
                nMig = n;
                _migrationCost = reportMigCost;
            }//if
        }//for
    }//for
    
    return bestPot;
}

Money SimpleLazyFacilityLocation::bestInstallationPot_(Node& nIns, Money& pAsn, vector<Demand>& DStar, SubstrateGraph::FlowMap& RStar, double lambda) {
    
    Money bestPot = -INFINITY;
    SubstrateGraph::FlowMap flows(network_, NO_FLOW);
    
    for (auto n = topology_.hostIt(); n != INVALID; ++n) {
        if (network_.residual(n) == 0) continue;
        
        vector<Demand> D;
        _clear(flows, network_);
        Money reportReasnCost = NO_COST;
        Money pot = insPot_(n, pAsn, flows, lambda, D, reportReasnCost);
        if (pot > bestPot) {
            bestPot = pot;
            _copy(flows, RStar);
            nIns = n;
            DStar = D;
            _reassignmentCost = reportReasnCost;
        }//if
    }//for
    
    return bestPot;
}

Money SimpleLazyFacilityLocation::bestEmigrationPot_(Facility v, Node& nEmg, SubstrateGraph::FlowMap& RStar, double lambda) {
    Money bestPot = -INFINITY;
    SubstrateGraph::FlowMap flows(network_, NO_FLOW);
    for (auto n = topology_.hostIt(); n != INVALID; ++n) {
        // If facility does not have capacity for a new demand,
        //there is no point to migrate it
        if (network_.residual(n) == 0) continue;
        // There is no point to migrate facility to its current location
        if (v.location() == network_.id(n)) continue;

        Money reportMigCost = NO_COST;
        _clear(flows, network_);
        Money pot = emgPot_(v, n, flows, lambda, reportMigCost);
        if (pot > bestPot) {
            bestPot = pot;
            _copy(flows, RStar);
            nEmg = n;
            _migrationCost = reportMigCost;
        }//if
    }//for
    
    return bestPot;
}

bool SimpleLazyFacilityLocation::arrival(Demand d, double lambda) {
    bool re = true;
    // Set query statistics zero
    _clearCostStats();
    
    // Add demand
    _d = d;
    mapping_.addDemand(_d);
    
    cout << "\tChecking assignment: ";
    clock_t start = clock();
    // Best current assignment
    Facility vAsn;
    SubstrateGraph::FlowMap RAsn(network_, NO_FLOW);
    Money pAsn = bestAssignment_(vAsn, _d, RAsn);
    cout << ((double)clock() - start) / CLOCKS_PER_SEC << " seconds" << endl;
    
    start = clock();
    cout << "\tChecking migration: ";
    // Best migration
    Facility vMig;
    Node     nMig;
    SubstrateGraph::FlowMap RMig(network_, NO_FLOW);
    Money eMig = bestMigrationPot_(vMig, nMig, RMig, lambda);
    cout << ((double)clock() - start) / CLOCKS_PER_SEC << " seconds" << endl;
    
    start = clock();
    cout << "\tChecking installation: ";
    // Best installation
    Node     nIns;
    std::vector<Demand> DRe;
    SubstrateGraph::FlowMap RIns(network_, NO_FLOW);
    Money eIns = bestInstallationPot_(nIns, pAsn, DRe, RIns, lambda);
    cout << ((double)clock() - start) / CLOCKS_PER_SEC << " seconds" << endl;

    // Check for installation
    if (eIns > -pAsn && eIns >= eMig) {
        cout << "\tInstall a facility at " << network_.id(nIns) << endl;
        Facility vIns = install_(nIns);
        
        cout << "\tAssign " << _d.identifier() << "(" << _d.source() << "," << _d.target()
            << ") to facility " << vIns.identifier() << " at " << network_.id(nIns) << endl;
        
        mapping_.assign(vIns, _d, network_, RIns);
        
        //Do the reassignments
        _reassignmentNum = DRe.size();
        mapping_.deassign(DRe, network_);
        for (auto i = 0; i < _reassignmentNum; i++) {
            cout << "\tReassign demand " << DRe[i].identifier() << " at (" << DRe[i].source() << "," << DRe[i].target() <<") to facility " << vIns.identifier() << " at " << vIns.location() << endl;
            mapping_.assign(vIns, DRe[i], network_, RIns);
        }//for
    }//if
    //Check for migration
    else if (eMig > -pAsn) {
        cout << "\tMigrate facility " << vMig.identifier() << " from " << vMig.location() << " to " << network_.id(nMig) << endl;
        _migrationNum = 1;
        mapping_.migrate(vMig, nMig, network_, RMig);
        vMig = mapping_.facilities_[vMig.identifier()];
        mapping_.assign (vMig, _d, network_, RMig);
        
    }//else if
    //Assign to best facility
    else if (!isinf(pAsn)) {
        cout << "\tAssign to facility " << vAsn.identifier() << " in " << vAsn.location() << endl;
        mapping_.assign(vAsn, _d, network_, RAsn);
    }//else
    else {
        cout << "\tRejected!" << endl;
        mapping_.remove(_d);
        re = false;
    }//else
    return re;
}

void SimpleLazyFacilityLocation::departure(Demand d, double lambda) {
    // Finding the demand for its Kariz, because Kariz is set in arrival and
    //it is not available to external
    d = mapping_.demand(d.identifier());
    
    // Set query statistics zero
    _clearCostStats();
    
    // Find demand's facility
    Facility v = mapping_.facility(d);
    
    // Remove demand and its assigned resources
    mapping_.deassign(d, network_);
    mapping_.remove(d);
    
    //Best emigration
    cout << "\tChecking Emigration: ";
    clock_t start = clock();
    Node     nEmg;
    SubstrateGraph::FlowMap REmg(network_, NO_FLOW);
    Money eEmg = bestEmigrationPot_(v, nEmg, REmg, lambda);
    cout << ((double)clock() - start) / CLOCKS_PER_SEC << " seconds" << endl;
    
    //Best removal
    cout << "\tChecking Removal: ";
    start = clock();
    Node nRmv;
    std::vector<Demand> DRe;
    std::vector<Facility> VRe;
    vector<vector<Arc> > RRmv;
    Money eRmv = rmvPot_(v, lambda, DRe, VRe, RRmv, _reassignmentCost);
    cout << ((double)clock() - start) / CLOCKS_PER_SEC << " seconds" << endl;
    
    //Check for removal
    if (eRmv > 0 && eRmv > eEmg) {
        //Do the reassignments
        cout << "Removal:" << endl;
        
        // Deassign demands
        mapping_.deassign(DRe, network_);
        _reassignmentNum = DRe.size();
        for (auto i = 0; i < _reassignmentNum; i++) {
            cout << "Demand " << DRe[i].identifier()
                 << "(" << DRe[i].source() << "," << DRe[i].target()
                 << ") to facility " << v.identifier() << " at " << v.location() << endl;
            mapping_.assign(VRe[i], DRe[i], network_, RRmv[i]);
        }//for
        
        //Remove facility
        cout << "\tFacility " << v.identifier() << " at location " << v.location() << " is removed!" << endl;
        remove_(v);
    }//if
    else if (eEmg > 0) {
        cout << "Emigration:" << endl;
        cout << "\tFacility " << v.identifier() << " at location " << v.location()
             << " is migrated to " << network_.id(nEmg) << "!" << endl;
        _migrationNum = 1;
        mapping_.migrate(v, nEmg, network_, REmg);
    }//else if
}

std::string SimpleLazyFacilityLocation::name() {
    return NAME;
}

Money SimpleLazyFacilityLocation::reassignmentCost() {
    return (_reassignmentNum > 0) ? _reassignmentCost : NO_COST;
}

size_t SimpleLazyFacilityLocation::reassignmentNum() {
    return _reassignmentNum;
}

Money  SimpleLazyFacilityLocation::migrationCost() {
    return (_migrationNum > 0) ? _migrationCost : NO_COST;
}

size_t SimpleLazyFacilityLocation::migrationNum() {
    return _migrationNum;
}
