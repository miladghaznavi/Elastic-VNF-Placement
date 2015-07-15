//
//  mapping.cpp
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 6/11/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "mapping.h"

using std::stringstream;
using std::invalid_argument;
using std::endl;

Mapping::Mapping() { }

void Mapping::trackFlow_(Node& src, Node& trg, SubstrateGraph& network, SubstrateGraph::FlowMap& flows, vector<Arc>& path) {
    Node next = trg;
    SubstrateGraph::InArcIt arc(network, trg);
    while (arc != INVALID && next != src) {
        if (flows[arc] != NO_FLOW) {
            path.push_back(arc);
            flows[arc] -= BANDWIDTH_UNIT;
            next = network.source(arc);
            arc = SubstrateGraph::InArcIt(network, next);
        }//if
        else {
            ++arc;
        }//else
    }//while
}

void Mapping::findFlowPath_(Facility& v, Demand& d, SubstrateGraph& network, SubstrateGraph::FlowMap& flows, vector<Arc>& path) {
    Node src = network.nodeFromId(d.source());
    Node trg = network.nodeFromId(d.target());
    Node nv  = network.nodeFromId(v.location());
    trackFlow_(nv, src, network, flows, path);
    trackFlow_(nv, trg, network, flows, path);
}

//void Mapping::trackFlow_(Node& node, SubstrateGraph& network, SubstrateGraph::FlowMap& flows, vector<Arc>& path) {
//    SubstrateGraph::InArcIt arc(network, node);
//    do {
//        if (flows[arc] != NO_FLOW) {
//            path.push_back(arc);
//            flows[arc] -= BANDWIDTH_UNIT;
//            arc = SubstrateGraph::InArcIt(network, network.source(arc));
//        }//if
//        else {
//            ++arc;
//        }//else
//    }//do
//    while(arc != INVALID);//while
//}
//
//void Mapping::findFlowPath_(Demand& d, SubstrateGraph& network, SubstrateGraph::FlowMap& flows, vector<Arc>& path) {
//    Node src = network.nodeFromId(d.source());
//    Node trg = network.nodeFromId(d.target());
//    trackFlow_(src, network, flows, path);
//    trackFlow_(trg, network, flows, path);
//}

Facility Mapping::facility(Demand d) {
    auto found = demands2FacsMap_.find(d.identifier());
    if (found == demands2FacsMap_.end()) {
        std::stringstream ss;
        ss << "Demand with id '" << d.identifier() << "' is not assigned to any facility!";
        throw std::logic_error(ss.str());
    }//if
    
    return facilities_[demands2FacsMap_[d.identifier()]];
}

int Mapping::facilityNum() {
    return (int)facilities_.size();
}

int Mapping::demandNum() {
    return (int)demands_.size();
}

Demand Mapping::demand(Identifier id) {
    auto pos = demands_.find(id);
    if (pos == demands_.end()) {
        std::stringstream ss;
        ss << "No demand with id '" << id << "' exists!";
        throw std::invalid_argument(ss.str());
    }//if
    
    return pos->second;
}

void Mapping::demands (Facility v, vector<Demand>& Dv) {
    Identifier vId = v.identifier();
    for (auto it = demands2FacsMap_.begin(); it != demands2FacsMap_.end(); it++)
        if (it->second == vId)
            Dv.push_back(demands_[it->first]);
}

void Mapping::addDemand(Demand d) {
    if (demands_.find(d.identifier()) != demands_.end()) {
        std::stringstream ss;
        ss << "A demand with id " << d.identifier() << " already exits in the map! This demand "
            << d.identifier() << "(" << d.source() << "," << d.target() << ") cannot be added!";
        throw std::invalid_argument(ss.str());
    }//if
    
    demands_.insert(std::make_pair(d.identifier(), d));
}

void Mapping::remove(Demand d) {
    demands_.erase(d.identifier());
    demands2FacsMap_.erase(d.identifier());
    demandFlows_.erase(d.identifier());
}

void Mapping::install(Facility v, SubstrateGraph& network) {
    // Installing a facility involves:
    // 1- Allocating resource of a node
    // 2- Adding to facilities_ map
    
    if (facilities_.find(v.identifier()) != facilities_.end()) {
        std::stringstream ss;
        ss  << "A facility with id " << v.identifier() << " already exits in the map! This facility at location "
            << v.location() << " cannot be added!";
        throw std::invalid_argument(ss.str());
    }//if
    if (network.residual(network.nodeFromId(v.location())) == 0) {
        std::stringstream ss;
        ss << "There is no residual capacity in node " << v.location() << "! Facility " << v.identifier() << " cannot be installed!";
        throw std::invalid_argument(ss.str());
    }//if
    if (v.identifier() < 0) {
        std::stringstream ss;
        ss << "Id of facility " << v.identifier() << " is not valid! This facility is requested to be installed in node" << v.location();
        throw std::invalid_argument(ss.str());
    }//if
    
    // 1- Allocating resource of a node
    network.allocate(network.nodeFromId(v.location()), FACILITY_REQ_CAP);
    
    // 2- Adding to facilities_ map
    facilities_.insert(std::make_pair(v.identifier(), v));
}

void Mapping::remove(Facility v, SubstrateGraph& network) {
    // Removing a facility involves:
    // 1- Make sure that no demand is still assigned to facility
    // 2- Releasing resource of a node
    // 3- Removing from facilities_
    
    // 1- Make sure that no demand is still assigned to facility
    Identifier vId = v.identifier();
    for (auto it = demands2FacsMap_.begin(); it != demands2FacsMap_.end(); it++) {
        if (it->second == vId) {
            std::stringstream ss;
            Demand d = demands_[it->first];
            ss  << "Facility " << vId << " at location '" << v.location()
                << "' has at least a demand assigned to it. Demand " << d.identifier() << "(" << d.source() << "," << d.target()  << ") is found!";
            throw std::invalid_argument(ss.str());
        }//if
    }//for
    
    // 2- Releasing resource of a node
    network.release(network.nodeFromId(v.location()), FACILITY_REQ_CAP);
    
    // 3- Removing from facilities_
    facilities_.erase(v.identifier());
}

void Mapping::assign(Facility v, Demand d, SubstrateGraph& network, vector<Arc>& flow) {
    // Assignment invlove:
    //  1- Making sure that both demand and facility already exist in maps, and match with existing ones
    //  2- Making sure that demand is not assigned to another facility
    //  3- Allocation of a unit of facility capacity to the demand
    //  4- Storing mapping of facility and demand
    //  5- Allocation of link resources of network for the demand's flows
    //  6- Storing flow of the demand
    
    Identifier dId = d.identifier();
    Identifier vId = v.identifier();
    if (dId == INVALID_ID) {
        throw std::invalid_argument("Not valid demand!");
    }//if
    if (vId == INVALID_ID) {
        throw std::invalid_argument("Not valid facility!");
    }//if
    
    //  1- Making sure that both demand and facility already exist in maps, and match with existing ones
    //      checking demand
    auto dFound = demands_.find(d.identifier());
    if (dFound == demands_.end()) {
        std::stringstream ss;
        ss  << "No demand exists like "
            << d.identifier() << "(" << d.source() << "," << d.target() << ")!";
        throw std::invalid_argument(ss.str());
    }//if
    if (dFound->second != d) {
        std::stringstream ss;
        ss  << "A demand with id " << dFound->second.identifier() << " found which does not match the assigning demand!" << endl;
        ss  << "Assigning demand is " << d.identifier()              << "(" << d.source()              << "," << d.target()              << ")!" << endl;
        ss  << "Demand found is "     << dFound->second.identifier() << "(" << dFound->second.source() << "," << dFound->second.target() << ")!";
        throw std::invalid_argument(ss.str());
    }//if
    //      checking facility
    auto vFound = facilities_.find(v.identifier());
    if (vFound == facilities_.end()) {
        std::stringstream ss;
        ss  << "No facility exists with id " << v.identifier()
            << " at location " << v.location() << endl;
        throw std::invalid_argument(ss.str());
    }//if
    if (vFound->second.location() != v.location()) {
        std::stringstream ss;
        ss  << "Location of assigning facility does not match with facility found" << endl;
        ss  << "Assigning facility is " << v.identifier()              << " at location " << v.location()              << endl;
        ss  << "found facility is "     << vFound->second.identifier() << " at location " << vFound->second.location() << "!";
        throw std::invalid_argument(ss.str());
    }//if
    
    //  2- Making sure that demand is not assigned to another facility
    auto mapFound = demands2FacsMap_.find(d.identifier());
    if (mapFound != demands2FacsMap_.end()) {
        auto fff = facilities_[mapFound->second];
        stringstream ss;
        ss  << "Assigning demand " << d.identifier() << "(" << d.source() << "," << d.target() << ")"
            << " is already assigned to facility " << fff.identifier() << " at location " << fff.location() << endl;
        throw std::invalid_argument(ss.str());
    }//if

    //  3- Allocation of a unit of facility capacity to the demand
    vFound->second.allocate();

    //  4- Storing mapping of facility and demand
    demands2FacsMap_.insert(std::make_pair(dId, vId));
    
    //  5- Allocation of link resources of network for the demand's flows
    for (Arc a1: flow) {
        Network::allocateArcRes(network, a1, BANDWIDTH_UNIT);
    }//for
    
    //  5- Storing flow of the demand
    demandFlows_.insert(std::make_pair(dId, flow));
}

void Mapping::assign(Facility v, Demand d, SubstrateGraph& network, SubstrateGraph::FlowMap& flows) {
    vector<Arc> path;
    findFlowPath_(v, d, network, flows, path);
    assign(v, d, network, path);
}

void Mapping::assign(Facility v, vector<Demand>& D, SubstrateGraph& network, SubstrateGraph::FlowMap& flows) {
    for (Demand d: D)
        assign(v, d, network, flows);
}

void Mapping::deassign(Demand d, SubstrateGraph& network) {
    // deassignment invlove:
    //  1- Removing mapping of facility and demand
    //  2- Releasing link resources of network for the demand's flows
    //  3- Releasing a unit of facility capacity which were assigned to the demand
    //  4- Removing flow of the demand
    
    //  Checking demand
    auto dFound = demands_.find(d.identifier());
    if (dFound == demands_.end()) {
        std::stringstream ss;
        ss  << "Cannot deassing! No demand exists like "
            << d.identifier() << "(" << d.source() << "," << d.target() << ")!";
        throw std::invalid_argument(ss.str());
    }//if
    if (dFound->second != d) {
        std::stringstream ss;
        ss  << "A demand with id " << dFound->second.identifier() << " found which does not match the assigning demand!" << endl;
        ss  << "Assigning demand is " << d.identifier()              << "(" << d.source()              << "," << d.target()              << ")!" << endl;
        ss  << "Demand found is "     << dFound->second.identifier() << "(" << dFound->second.source() << "," << dFound->second.target() << ")!";
        throw std::invalid_argument(ss.str());
    }//if

    Identifier dId = d.identifier();
    auto map = demands2FacsMap_.find(dId);
    Identifier vId = map->second;
    
    // 1- Removing mapping of facility and demand
    demands2FacsMap_.erase(map);
    
    // 2- Releasing link resources of network for the demand's flows
    auto flow = demandFlows_[dId];
    for (Arc a: flow)
        Network::releaseArcRes(network, a, BANDWIDTH_UNIT);
    
    // 3- Releasing a unit of facility capacity which were assigned to the demand
    facilities_[vId].release();
    
    // 4- Removing flow of the demand
    demandFlows_.erase(dId);
}

void Mapping::deassign(vector<Demand>& D, SubstrateGraph& network) {
    for (Demand d : D)
        deassign(d, network);
}

//void Mapping::reassign(Demand d, Facility v, SubstrateGraph& network, vector<Arc>& arcs) {
//    deassign(d, network);
//    assign  (v, d, network, arcs);
//}
//
//void Mapping::reassign(Demand d, Facility v, SubstrateGraph& network, SubstrateGraph::FlowMap& flow) {
//    deassign(d, network);
//    assign  (v, d, network, flow);
//}

void Mapping::removeNetworkFootprint(Demand d, SubstrateGraph& network) {
    auto flow = demandFlows_[d.identifier()];
    for (Arc a1: flow)
        Network::releaseArcRes(network, a1, BANDWIDTH_UNIT);
}

void Mapping::removeNetworkFootprint(vector<Demand>& D, SubstrateGraph& network) {
    for (size_t i = 0; i < D.size(); i++)
        removeNetworkFootprint(D[i], network);
}

void Mapping::removeNetworkFootprint(Facility v, SubstrateGraph& network) {
    Identifier vId = v.identifier();
    for (auto it = demands2FacsMap_.begin(); it != demands2FacsMap_.end(); it++)
        if (it->second == vId)
            removeNetworkFootprint(demands_[it->first], network);
}

void Mapping::backNetworkFootprint(Demand d, SubstrateGraph& network) {
    auto flow = demandFlows_[d.identifier()];
    for (Arc a: flow)
        Network::allocateArcRes(network, a, BANDWIDTH_UNIT);
}

void Mapping::backNetworkFootprint(vector<Demand>& D, SubstrateGraph& network) {
    for (size_t i = 0; i < D.size(); i++)
        backNetworkFootprint(D[i], network);
}

void Mapping::backNetworkFootprint(Facility v, SubstrateGraph& network) {
    Identifier vId = v.identifier();
    for (auto it = demands2FacsMap_.begin(); it != demands2FacsMap_.end(); it++)
        if (it->second == vId)
            backNetworkFootprint(demands_[it->first], network);
}

void Mapping::migrate(Facility v, Node n, SubstrateGraph& network, SubstrateGraph::FlowMap& flows) {
    // A Migration invloves:
    // 1- Removing current flow of the demands which are assigned to the facility
    // 2- Removing current host resources of v
    // 3- Allocating n's reourses to v
    // 4- Allocating new flows to the facility
    // 5- Update location of the facility
    
    // 1- Removing current flow of the demands which are assigned to the facility
    Identifier vId = v.identifier();
    vector<Demand> demands;
    for (auto it = demands2FacsMap_.begin(); it != demands2FacsMap_.end(); it++) {
        if (it->second == vId) {
            Demand d = demands_[it->first];
            demands.push_back(d);
            auto flow = demandFlows_.find(d.identifier());
            for (Arc a: flow->second)
                Network::releaseArcRes(network, a, BANDWIDTH_UNIT);
            demandFlows_.erase(flow);
        }//if
    }//for
    
    // 2- Removing current host resources of v
    network.release(network.nodeFromId(v.location()), FACILITY_REQ_CAP);
    
    // 3- Allocating n's reourses to v
    network.allocate(n, FACILITY_REQ_CAP);
    
    // 4- Allocating new flows to the facility
    vector<Arc> path;
    Identifier dId;
    v.location(network.id(n));
    for (Demand d: demands) {
        path.clear();
        findFlowPath_(v, d, network, flows, path);
        dId = d.identifier();
        
        demandFlows_.insert(std::make_pair(dId, path));
        for (Arc a: path)
            Network::allocateArcRes(network, a, BANDWIDTH_UNIT);
    }//for
    
    // 5- Update location of the facility
    facilities_[vId].location(network.id(n));
}

void Mapping::flow(Demand d, vector<Arc>& path) {
    auto found = demandFlows_.find(d.identifier());
    if (found == demandFlows_.end()) {
        std::stringstream ss;
        ss << "No assignment exists for demand with id '" << d.identifier() << "'";
        throw std::logic_error(ss.str());
    }//if
    path = found->second;
}

void Mapping::flow(Demand d, SubstrateGraph::FlowMap& path) {
    for (Arc a: demandFlows_[d.identifier()])
        path[a] += BANDWIDTH_UNIT;
}

void Mapping::flow(vector<Demand>& D, vector<Arc>& path) {
    vector<Arc> dPath;
    for (size_t i = 0; i < D.size(); ++i) {
        dPath.clear();
        flow(D[i], dPath);
        path.insert(path.end(), dPath.begin(), dPath.end());
    }//for
}

void Mapping::flow(Facility v, unordered_map<Identifier, vector<Arc> >& paths) {
    for (auto it = demands2FacsMap_.begin(); it != demands2FacsMap_.end(); ++it) {
        if (it->second == v.identifier()) {
            Demand d = demands_[it->first];
            vector<Arc> path;
            flow(d, path);
            paths.insert(std::make_pair(d.identifier(), path));
        }//
    }//for
}

void Mapping::flow(Facility v, vector<Arc>& paths) {
    for (auto it = demands2FacsMap_.begin(); it != demands2FacsMap_.end(); ++it) {
        if (it->second == v.identifier()) {
            auto path = demandFlows_[it->first];
            paths.insert(paths.end(), path.begin(), path.end());
        }//
    }//for
}

void Mapping::flow(Facility v, SubstrateGraph::FlowMap& re) {
    for (auto it = demands2FacsMap_.begin(); it != demands2FacsMap_.end(); ++it) {
        if (it->second == v.identifier()) {
            Demand d = demands_[it->first];
            flow(d, re);
        }//
    }//for
}

