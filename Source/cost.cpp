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

Cost::Cost(Parameters& params, SubstrateGraph& network, KAryFatTree& topology) {
    _params = params;
    _findShortestPaths(network, topology);
}

void Cost::_findShortestPaths(SubstrateGraph& network, KAryFatTree& topology) {
    //Init dijkstras
    int hostNum = topology.hostNum();
    int firstHostId = topology.id(topology.firstHost());
    _shortestPaths = new double*[hostNum];

    for (auto src = topology.hostIt(); src != INVALID; ++src) {
        int srcIndex = topology.id(src) - firstHostId;
        _shortestPaths[srcIndex] = new double[hostNum];

        ShortestPath dijkstra(network, network._edgeWeight);
        SubstrateGraph::DistMap dist(network);
        dijkstra.distMap(dist);
        dijkstra.run(src);
        
        for (auto dst = topology.hostIt(); dst != INVALID; ++dst) {
            int dstIndex = topology.id(dst) - firstHostId;
            _shortestPaths[srcIndex][dstIndex] = dist[dst];
        }//for
    }//for
}

Arc Cost::_findArc(Node& host, SubstrateGraph& network, KAryFatTree& topology) {
    Edge e = topology.hostEdge(host);
    return topology.arcFromId(topology.id(e) * 2);
}

Money Cost::f() {
    return _params.hostCostInCentsPerTimeSlot;
}

Money Cost::g() {
    return _params.bandwidthUnitCostInCentsPerTimeSlot;
}

Money Cost::k(Node src, Node trg, SubstrateGraph& network, KAryFatTree& topology) {
    if (!topology.isHost(src) || ! topology.isHost(trg)) {
        throw std::logic_error("Both source and target most be host nodes!");
    }//if
    
    int firstHostIndex = topology.id(topology.firstHost());
    int srcIndex = topology.id(src) - firstHostIndex;
    int trgIndex = topology.id(trg) - firstHostIndex;
    
    double p = _shortestPaths[srcIndex][trgIndex];
    Arc srcA = _findArc(src, network, topology);
    Arc trgA = _findArc(trg, network, topology);
    Capacity minCap = std::min(network.residual(srcA), network.residual(trgA));
    
    Money re;
    if (minCap != 0)
        re = ((double)_params.vnfRamSizeInTermsOfBandwidthUnit * _params.bandwidthUnitCostInCentsPerTimeSlot * p) /
                (minCap * _params.timeSlotInSecond);
    else
        re = INFINITY;
    
    return re;
}

Money Cost::h(Node src, Node trg, SubstrateGraph& network, KAryFatTree& topology) {
    return k(src, trg, network, topology) / _params.vnfMaxCapInTermsOfDemandsNum;
}
