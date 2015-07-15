//
//  substrategraph.cpp
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 5/2/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "substrategraph.h"
#include <lemon/core.h>

SubstrateGraph::SubstrateGraph() :
                                _nodeCapacity (*this, NO_CAPACITY),
                                _nodeResidual (*this, NO_CAPACITY),
                                _edgeCapacity (*this, NO_CAPACITY),
                                _arcCapacity  (*this, NO_CAPACITY),
                                _edgeResidual (*this, NO_CAPACITY),
                                _arcResidual  (*this, NO_CAPACITY),
                                _edgeWeight   (*this, NO_WEIGHT  ),
                                _arcWeight    (*this, NO_WEIGHT  ) { };

SubstrateGraph::SubstrateGraph(Graph& graph) : SubstrateGraph() {
    this->copyFrom(graph);
}

Capacity SubstrateGraph::capacity (Node n) {
    return _nodeCapacity[n];
}

Capacity SubstrateGraph::capacity(Edge e) {
    return _edgeCapacity[e];
}

Capacity SubstrateGraph::capacity(Arc a) {
    return _arcCapacity[a];
}

Weight SubstrateGraph::weight(Edge e) {
    return _edgeWeight[e];
}

Weight SubstrateGraph::weight(Arc a) {
    return _arcWeight[a];
}

Capacity SubstrateGraph::residual(Node n) {
    return _nodeResidual[n];
}

Capacity SubstrateGraph::residual(Edge e) {
    return _edgeResidual[e];
}

Capacity SubstrateGraph::residual(Arc a) {
    return _arcResidual[a];
}

Capacity SubstrateGraph::allocated(Node n) {
    return _nodeCapacity[n] - _nodeResidual[n];
}

Capacity SubstrateGraph::allocated(Edge e) {
    return _edgeCapacity[e] - _edgeResidual[e];
}

Capacity SubstrateGraph::allocated(Arc a) {
    return _arcCapacity[a] - _arcResidual[a];
}

void SubstrateGraph::capacity(Node n, Capacity amount) {
    _nodeCapacity[n] = amount;
    _nodeResidual[n] = amount;
}

void SubstrateGraph::capacity(Edge e, Capacity amount) {
    _edgeCapacity[e] = amount;
    _edgeResidual[e] = amount;
}

void SubstrateGraph::capacity(Arc a, Capacity amount) {
    _arcCapacity[a] = amount;
    _arcResidual[a] = amount;
}

void SubstrateGraph::weight(Edge e,  Weight w) {
    _edgeWeight[e] = w;
}

void SubstrateGraph::weight(Arc a, Capacity amount) {
    _arcWeight[a] = amount;
}

void SubstrateGraph::allocate(Node n, Capacity amount) {
    if (amount > residual(n)) {
        std::stringstream ss;
        ss  << "Cannot allocate more than residual capacity. Requested allocation is '"
        << amount << "' and residual capacity is '" << residual(n) << "'!";
        throw std::invalid_argument(ss.str());
    }//if
    _nodeResidual[n] -= amount;
}

void SubstrateGraph::allocate(Edge e, Capacity amount) {
    if (amount > residual(e)) {
        std::stringstream ss;
        ss  << "Cannot allocate more than residual capacity. Requested allocation is '"
            << amount << "' and residual capacity is '" << residual(e) << "'!";
        throw std::invalid_argument(ss.str());
    }//if
    _edgeResidual[e] -= amount;
}

void SubstrateGraph::allocate(Arc a, Capacity amount) {
    if (amount > residual(a)) {
        std::stringstream ss;
        ss  << "Cannot allocate more than residual capacity. Requested allocation is '"
        << amount << "' and residual capacity is '" << residual(a) << "'!";
        throw std::invalid_argument(ss.str());
    }//if
    _arcResidual[a] -= amount;
}

void SubstrateGraph::release(Node n, Capacity release) {
    if (allocated(n) < release) {
        std::stringstream ss;
        ss  << "Cannot release more than what is allocated. Requested release is '"
        << release << "' and allocated capacity is '" << allocated(n) << "'!";
        throw std::invalid_argument(ss.str());
    }//if
    _nodeResidual[n] += release;
}

void SubstrateGraph::release(Edge e,  Capacity release) {
    if (allocated(e) < release) {
        std::stringstream ss;
        ss  << "Cannot release more than what is allocated. Requested release is '"
        << release << "' and allocated capacity is '" << allocated(e) << "'!";
        throw std::invalid_argument(ss.str());
    }//if
    _edgeResidual[e] += release;
}

void SubstrateGraph::release(Arc a,  Capacity release) {
    if (allocated(a) < release) {
        std::stringstream ss;
        ss  << "Cannot release more than what is allocated. Requested release is '"
        << release << "' and allocated capacity is '" << allocated(a) << "'!";
        throw std::invalid_argument(ss.str());
    }//if
    _arcResidual[a] += release;
}

void SubstrateGraph::allocateAll(Node n) {
    allocate(n, residual(n));
}

void SubstrateGraph::allocateAll(Edge e) {
    allocate(e, residual(e));
}

void SubstrateGraph::allocateAll(Arc a) {
    allocate(a, residual(a));
}

void SubstrateGraph::releaseAll(Node n) {
    release(n, allocated(n));
}

void SubstrateGraph::releaseAll(Edge e) {
    release(e, allocated(e));
}

void SubstrateGraph::releaseAll(Arc a) {
    release(a, allocated(a));
}

double SubstrateGraph::utilization(Node n) {
    return ((double) allocated(n)) / ((double)capacity(n));
}

double SubstrateGraph::utilization(Edge e) {
    return ((double) allocated(e)) / ((double) capacity(e));
}

double SubstrateGraph::utilization(Arc a) {
    return ((double) allocated(a)) / ((double) capacity(a));
}

double SubstrateGraph::nodesUtilization() {
    double re = 0;
    
    for (NodeIt it(*this); it != INVALID; ++it) {
        re += utilization(it);
    }//for
    
    return re / nodeNum();
}

double SubstrateGraph::edgesUtilization() {
    double re = 0;
    
    for (EdgeIt it(*this); it != INVALID; ++it) {
        re += utilization(it);
    }//for
    
    return re / edgeNum();
}

double SubstrateGraph::arcsUtilization() {
    double re = 0;
    
    for (ArcIt it(*this); it != INVALID; ++it) {
        re += utilization(it);
    }//for
    
    return re / arcNum();
}

void SubstrateGraph::copyTo(SubstrateGraph& to) {
    int n = nodeNum();
    int m = edgeNum();
    int p = arcNum();
    
    for (int i = 0; i < n; i++) {
        Node ii = to.addNode();
        to._nodeCapacity[ii] = _nodeCapacity[ii];
        to._nodeResidual[ii] = _nodeResidual[ii];
    }//for
    
    for (int i = 0; i < m; i++) {
        Edge e = edgeFromId(i);
        to.addEdge(u(e), v(e));
        to._edgeCapacity[e] = _edgeCapacity[e];
        to._edgeResidual[e] = _edgeResidual[e];
        to._edgeWeight[e]   = _edgeWeight[e];
    }//for
    
    for (int i = 0; i < p; i++) {
        Arc a = arcFromId(i);
        to._arcCapacity[a] = _arcCapacity[a];
        to._arcResidual[a] = _arcResidual[a];
        to._arcWeight[a]   = _arcWeight[a];
    }//for
}

void SubstrateGraph::copyFrom(Graph& from) {
    this->clear();
    
    int n = from.nodeNum();
    int m = from.edgeNum();
    for (int i = 0; i < n; i++) {
        addNode();
    }//for
    
    for (int i = 0; i < m; i++) {
        Edge e = from.edgeFromId(i);
        addEdge(from.u(e), from.v(e));
    }//for
}

