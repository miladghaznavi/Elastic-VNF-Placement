//
//  cost.cpp
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 5/28/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "cost.h"
#include <lemon/dijkstra.h>
#include <stdexcept>
#include <math.h>

Cost::Cost(Parameters params, FlowGraph& network, KAryFatTree& topology) {
    _params = params;
    _findShortestPaths(network, topology);
}

void Cost::_findShortestPaths(FlowGraph& network, KAryFatTree& topology) {
    //Init dijkstras
    int hostNum = topology.hostNum();
    int firstHostId = topology.id(topology.firstHost());
    _shortestPaths = new double*[hostNum];
    ShortestPath dijkstra(network, network._edgeWeight);
    FlowGraph::DistMap dist(network);
    dijkstra.distMap(dist);
    
    for (auto src = topology.hostIt(); src != INVALID; ++src) {
        int srcIndex = topology.id(src) - firstHostId;
        _shortestPaths[srcIndex] = new double[hostNum];
        
        Node hostInNetwork = network.nodeFromId(topology.id(src));
        dijkstra.run(hostInNetwork);
        
        for (auto dst = topology.hostIt(); dst != INVALID; ++dst) {
            int dstIndex = topology.id(dst) - firstHostId;
            _shortestPaths[srcIndex][dstIndex] = dist[dst];
        }//for
    }//for
}

Edge Cost::_findEdge(Node host, FlowGraph& network, KAryFatTree& topology){
    int firstEdgeSwitchId = topology.id(topology.aggSwitchIt()) + 1;
    int lastEdgeSwitchId = topology.id(topology.edgeSwitchIt());
    
    Edge re = INVALID;
    for (FlowGraph::IncEdgeIt e(network, host); re == INVALID && e != INVALID; ++e) {
        Node u = network.u(e); int uId = network.id(u);
        Node v = network.v(e); int vId = network.id(v);
        if (u != host && uId >= firstEdgeSwitchId && uId <= lastEdgeSwitchId) {
            re = e;
        }//if
        else if (v != host && vId >= firstEdgeSwitchId && vId <= lastEdgeSwitchId) {
            re = e;
        }//else if
    }//for
    
    return re;
}

Node Cost::_findHost(Node n, FlowGraph& network, KAryFatTree& topology) {
    int firstHostId = topology.id(topology.edgeSwitchIt()) + 1;
    int lastHostId  = topology.id(topology.hostIt());
    
    Node re = INVALID;
    for (FlowGraph::IncEdgeIt e(network, n); re == INVALID && e != INVALID; ++e) {
        Node u = network.u(e); int uId = network.id(u);
        Node v = network.v(e); int vId = network.id(v);
        if (u != n && uId >= firstHostId && uId <= lastHostId) {
            re = u;
        }//if
        else if (v != n && vId >= firstHostId && vId <= lastHostId) {
            re = v;
        }//else if
    }//for
    
    return re;
}

Money Cost::f() {
    return _params.hostCostInCentsPerTimeSlot;
}

Money Cost::g() {
    return _params.bandwidthUnitCostInCentsPerTimeSlot;
}

//TODO:
// We already implemented the case that we assume the ideal case for migration:
//  - We are only concern about the source host, and target host links to ToRs.
//      - We assume that the maximum flow that we can send is minimum available bandwidth over these two links.
//  - We find the shortest path and send the flow over shortest path
Money Cost::k(Node src, Node trg, FlowGraph& network, KAryFatTree& topology) {
    //Find the minimum cap on links
    Node srcHost = _findHost(src, network, topology);
    Node trgHost = _findHost(trg, network, topology);
    
    Edge srcE = _findEdge(srcHost, network, topology);
    Edge trgE = _findEdge(trgHost, network, topology);
    
    if (srcHost == INVALID || trgHost == INVALID) {
        throw std::logic_error("Both source and target most be facility nodes!");
    }//if
    
//    int srcIndex = topology.hostNum() - (topology.id(srcHost) - topology.id(topology.edgeSwitchIt()));
    int firstHostIndex = topology.id(topology.firstHost());
    int srcIndex = topology.id(srcHost) - firstHostIndex;
    int trgIndex = topology.id(trgHost) - firstHostIndex;
    
    double p = _shortestPaths[srcIndex][trgIndex];
    Capacity minCap = std::min(network.residual(srcE), network.residual(trgE));
    
    Money re;
    if (minCap != 0)
        re = ((double)_params.vnfRamSizeInTermsOfBandwidthUnit * _params.bandwidthUnitCostInCentsPerTimeSlot * p) /
                (minCap * _params.timeSlotInSecond);
    else
        re = INFINITY;
    
    return re;
}

Money Cost::h(Node src, Node trg, FlowGraph& network, KAryFatTree& topology) {
    return k(src, trg, network, topology) / _params.vnfMaxCapInTermsOfDemandsNum;
}
