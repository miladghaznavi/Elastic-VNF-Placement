//
//  flowgraph.cpp
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 5/3/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "flowgraph.h"
#include <sstream>
#include <algorithm>
#include <math.h>

FlowGraph::FlowGraph() :
                flow_          (*this, NO_FLOW),
                supply_        (*this, NO_SUPPLY),
                demandId_      (*this, INVALID_ID),
                isKariz_       (*this, NOT_KARIZ),
                isFacInstalled_(*this, NO_FACILITY){
    lastFacilityNode_ =
    lastOrdinaryNode_ = INVALID;
    facilityGenerated_ = false;
}

FlowGraph::FlowGraph(Graph& graph) : FlowGraph() {
    this->copyFrom(graph);
}

Node FlowGraph::_demandKariz(Demand d, Edge& srcEdge, Edge& trgEdge, Identifier demandId) {
    // We find the kariz belong to demandId using three characteristics
    //  1- A Kariz is only connected to its demand source and target
    //  2- IsKariz is true for this node
    //  3- A Kariz has the same id as the demand d.
    Node src = nodeFromId(d.source());
    Node trg = nodeFromId(d.target());
    
    Node kariz = INVALID;
    for (OutArcIt e1(*this, src); e1 != INVALID; ++e1) {
        for (OutArcIt e2(*this, trg); e2 != INVALID; ++e2) {
            Node tmp = this->target(e1);
            if (tmp == this->target(e2) && isKariz_[tmp] && demandId_[tmp] == demandId) {
                kariz = tmp;
                srcEdge = e1; trgEdge = e2;
                break;
            }//if
        }//if
    }//for
    
    return kariz;
}

void FlowGraph::generateFacilityNodes(Node hostBeg, Node hostEnd, Facility facility) {
    if (facilityNodesGenerated()) {
        throw std::logic_error("Facility nodes are generated only once! These nodes have been generated before!");
    }//if
    
    if (nodeNum() == 0) {
        throw std::logic_error("No node exists!");
    }//if
    
    facilityGenerated_ = true;
    lastOrdinaryNode_ = nodeFromId(nodeNum() - 1);
    double edgeCap = 2 * BANDWIDTH_UNIT * facility.capacity();
    for(NodeRangeIt host(*this, hostEnd, nodeFromId(id(hostBeg) - 1)); host != INVALID; ++host) {
        int count = capacity(host);
        
        for (int i = 0; i < count; i++) {
            Node facilityNode = addNode();
            
            Edge e = addEdge(facilityNode, host);
            auto eId = id(e);
            
            Arc a1 = arcFromId(2 * eId);
            Arc a2 = arcFromId(2 * eId + 1);
            if (id(source(a1)) > id(target(a1))) {
                Arc tmp = a1;
                a1 = a2;
                a2 = tmp;
            }//if
            
            weight  (a1, INFINITE_WEIGHT);
            capacity(a1, NO_CAPACITY);
            
            weight  (a2, NO_WEIGHT);
            capacity(a2, edgeCap);
        }//for
    }//for
    lastFacilityNode_ = nodeFromId(nodeNum() - 1);
}

bool FlowGraph::facilityNodesGenerated() {
    return facilityGenerated_;
}

void FlowGraph::addDemand(Demand& d) {
    if (!facilityNodesGenerated()) {
        throw std::logic_error("Before calling addDemand function, facilities must be generated (generateFacilityNodes must be called)!");
    }//if
    if (d.identifier() == INVALID_ID) {
        throw std::invalid_argument("Demand's id is not valid.");
    }//if
    
    Node src, trg, kariz;
    Edge srcEdge, trgEdge;
    Arc  srcArc1, trgArc1, srcArc2, trgArc2;
    
    src = this->nodeFromId(d.source());
    trg = this->nodeFromId(d.target());
    
    //Find a free kariz node to assign
    kariz = _demandKariz(d, srcEdge, trgEdge, INVALID_ID);
    
    if (kariz == INVALID) {
        //If no free kariz is found, add kariz and arcs
        kariz   = this->addNode();
        isKariz_[kariz] = true;
        // If src and trg are different nodes, we add two edges
        if (src != trg) {
            srcEdge = this->addEdge(src, kariz);
            trgEdge = this->addEdge(trg, kariz);
        }//if
        else {
            //If src and trg are same, we add one edge
            srcEdge = trgEdge = this->addEdge(src, kariz);
        }//else
    }//if
    
    //Arcs:
    srcArc1 = arcFromId(2 * id(srcEdge) + 1);
    srcArc2 = arcFromId(2 * id(srcEdge));
    if (id(source(srcArc1)) > id(target(srcArc1)))
        std::swap(srcArc1, srcArc2);
    
    trgArc1 = arcFromId(2 * id(trgEdge) + 1);
    trgArc2 = arcFromId(2 * id(trgEdge));
    if (id(source(trgArc1)) > id(target(trgArc1)))
        std::swap(trgArc1, trgArc2);
    
    //set demand d's kariz id
    d.kariz(id(kariz));
//    supply_  [kariz] = KARIZ_DEMAND_VAL;
    demandId_[kariz] = d.identifier();
    
    if (src != trg) {
        //The capacity of these arcs are set to a single unit of bandwidth.
        capacity(srcArc1, BANDWIDTH_UNIT);
        capacity(trgArc1, BANDWIDTH_UNIT);
    }//if
    else {
        //The capacity of these arcs are set to two unit of bandwidth.
        capacity(srcArc1, 2 * BANDWIDTH_UNIT);
    }//else
    
    // The capacity of oposite direction is set to zero
    //to make sure no flow initiate from a kariz
    capacity(srcArc2, NO_CAPACITY);
    capacity(trgArc2, NO_CAPACITY);
    
    //The weights of these arcs are set to 0 to add no extra cost
    weight(srcArc1, NO_WEIGHT);
    weight(trgArc1, NO_WEIGHT);
    weight(srcArc2, INFINITE_WEIGHT);
    weight(trgArc2, INFINITE_WEIGHT);
    
    capacity(srcEdge, BANDWIDTH_UNIT);
    capacity(trgEdge, BANDWIDTH_UNIT);
    weight(srcEdge, NO_WEIGHT);
    weight(trgEdge, NO_WEIGHT);
}

void FlowGraph::_invalidateNode(Node& n) {
//    supply_  [n] = NO_SUPPLY;
    demandId_[n] = INVALID_ID;
}

void FlowGraph::_invalidateEdge(Edge& e) {
    weight(e, INFINITE_WEIGHT);
    releaseAll(e);
    capacity(e, NO_CAPACITY);
}

void FlowGraph::_invalidateArc (Arc& a) {
    weight(a, INFINITE_WEIGHT);
    releaseAll(a);
    capacity(a, NO_CAPACITY);
}

void FlowGraph::removeDemand(Demand& d) {
    //  We actually do not remove the corresponding kariz node and arcs from the graph, becuase it is not efficient.
    //Instead, we invalidate the related kariz node and make it free candidate for the next demand that might come to
    //the same source and target pair.
    
    Node kariz = nodeFromId(d.kariz());
    if (kariz == INVALID) {
        std::stringstream ss;
        ss << "No demand with kariz, source, and target nodes "
                << d.kariz()  << ","
                << d.source() << ", and "
                << d.target() << ", respectively exist!";
        throw std::invalid_argument(ss.str());
    }//if
    if (!isKariz_[kariz]) {
        std::stringstream ss;
        ss << "No valid kariz: " << id(kariz);
        throw std::invalid_argument(ss.str());
    }//if
    
    //Invalidate srcEdge, trgEdge, srcArc, and trgArc
    for (IncEdgeIt e(*this, kariz); e != INVALID; ++e) {
        _invalidateEdge(e);
        Arc a = arcFromId(2 * id(e) + 1);
        if (id(source(a)) > id(target(a)))
            a = arcFromId(2 * id(e));
        _invalidateArc(a);
    }//for
    
    //Invalidate kariz
    _invalidateNode(kariz);
}

FlowGraph::NodeRangeIt FlowGraph::facilityNodeIt() {
    return FlowGraph::NodeRangeIt(*this, lastFacilityNode_, lastOrdinaryNode_);
}

Supply FlowGraph::supply(Node n) {
    return supply_[n];
}

void FlowGraph::supply(Node n, Supply supply) {
    supply_[n] = supply;
}

void FlowGraph::copyTo(FlowGraph& to) {
    SubstrateGraph::copyTo(to);
    for (int i = 0; i < nodeNum(); i++) {
        Node ii = nodeFromId(i);
        to.supply_[ii] = supply_[ii];
        to.demandId_[ii] = demandId_[ii];
        to.isFacInstalled_[ii] = isFacInstalled_[ii];
        to.isKariz_[ii] = isKariz_[ii];
        to.demandId_[ii] = demandId_[ii];
    }//for
    
    for (int i = 0; i < arcNum(); i++) {
        Arc a = arcFromId(i);
        to.flow_[a] = flow_[a];
    }//for
    
    to.facilityGenerated_ = this->facilityGenerated_;
    to.lastFacilityNode_ = this->lastFacilityNode_;
    to.lastOrdinaryNode_ = this->lastOrdinaryNode_;
}