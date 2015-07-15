//
//  network.cpp
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 6/11/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "network.h"

using std::cout;
using std::endl;

void Network::normalizeArcCap(SubstrateGraph& network, float factor) {
    // Set arc propoerties from edge properties
    for (EdgeIt e(network); e != INVALID; ++e) {
        auto eId = network.id(e);
        auto cap = network.capacity(e) / factor;
        auto wei = network.weight(e);
        
        Arc a1 = network.arcFromId(2 * eId);
        Arc a2 = network.arcFromId(2 * eId + 1);
        
        network.capacity(a1, cap);
        network.capacity(a2, cap);
        network.weight(a1, wei);
        network.weight(a2, wei);
    }//for
}

void Network::normalizeNodeCap(SubstrateGraph& network, float factor) {
    for (NodeIt n(network); n != INVALID; ++n) {
        network.capacity(n, floor(network.capacity(n) / factor));
    }//for
}

void Network::addFootPrint (SubstrateGraph& network, SubstrateGraph::ArcCapacityMap& residual, Arc& a1, Capacity amount) {
    Identifier aId = network.id(a1);
    Arc a2 = network.arcFromId(aId / 2 * 2 + 1);
    if (network.id(a2) == network.id(a1)) {
        a2 = network.arcFromId(aId / 2 * 2);
    }//if
    
    // Allocate the bandwidth on arc
    residual[a1] -= amount;
    residual[a2] -= amount;
}

void Network::removeFootPrint (SubstrateGraph& network, SubstrateGraph::ArcCapacityMap& residual, Arc& a1, Capacity amount) {
    Identifier aId = network.id(a1);
    Arc a2 = network.arcFromId(aId / 2 * 2 + 1);
    if (network.id(a2) == network.id(a1)) {
        a2 = network.arcFromId(aId / 2 * 2);
    }//if
    
    // Release the bandwidth on arc
    residual[a1] += amount;
    residual[a2] += amount;
}

void Network::allocateArcRes (SubstrateGraph& network, Arc& a1, Capacity amount) {
    Identifier aId = network.id(a1);
    Arc a2 = network.arcFromId(aId / 2 * 2 + 1);
    if (network.id(a2) == network.id(a1)) {
        a2 = network.arcFromId(aId / 2 * 2);
    }//if
    
    // Allocate the bandwidth on arc
    network.allocate(a1, amount);
    network.allocate(a2, amount);
}

void Network::releaseArcRes  (SubstrateGraph& network, Arc& a1, Capacity amount) {
    Identifier aId = network.id(a1);
    Arc a2 = network.arcFromId(aId / 2 * 2 + 1);
    if (network.id(a2) == network.id(a1)) {
        a2 = network.arcFromId(aId / 2 * 2);
    }//if
    
    // Allocate the bandwidth on arc
    network.release(a1, amount);
    network.release(a2, amount);
}

void Network::printArcs(SubstrateGraph& network) {
    for (ArcIt a(network); a!= INVALID; ++a) {
        cout << network.id(a) << "(" << network.id(network.source(a)) << "," << network.id(network.target(a)) << "):\t" << network.residual(a) << endl;
    }//for
}

void Network::printAllocatedArcs (SubstrateGraph& network) {
    bool found = false;
    
    for (ArcIt a(network); a!= INVALID; ++a) {
        if (network.allocated(a) > 0) {
            //print header
            if (!found) {
                cout << "Arc\t\t" << "Allocated\t" << "Residual" << endl;
                found = true;
            }//if
            
            cout << network.id(a) << "("    << network.id(network.source(a)) << "," << network.id(network.target(a))
                                  << "):\t" << network.allocated(a)
                                  << "\t\t"   << network.residual(a) << endl;
        }//if
    }//for
    
    if (!found) {
        cout << "No allocated arc!" << endl;
    }//if
}

void Network::printNodes(SubstrateGraph& network) {
    for (NodeIt n(network); n != INVALID; ++n) {
        cout << network.id(n) << "\t: " << network.residual(n) << endl;
    }//for
}

void Network::printAllocatedNodes(SubstrateGraph& network) {
    bool found = false;

    for (NodeIt n(network); n!= INVALID; ++n) {
        if (network.allocated(n) > 0) {
            //print header
            if (!found) {
                cout << "Node\t" << "Allocated\t" << "Residual" << endl;
                found = true;
            }//if
            
            cout << network.id(n)
                 << ":\t"  << network.allocated(n)
                 << "\t\t" << network.residual(n) << endl;
        }//if
    }//for
    
    if (!found) {
        cout << "No allocated node!" << endl;
    }//if
}

void Network::addFlow(SubstrateGraph& network, SubstrateGraph::ArcMap<Flow>& flows) {
    for (ArcIt a(network); a != INVALID; ++a)
        Network::allocateArcRes(network, a, flows[a]);
}

void Network::removeFlow(SubstrateGraph& network, SubstrateGraph::ArcMap<Flow>& flows) {
    for (ArcIt a(network); a != INVALID; ++a)
        Network::releaseArcRes(network, a, flows[a]);
}

void Network::printFlow(SubstrateGraph& network, SubstrateGraph::ArcMap<Flow>& flows) {
    for (ArcIt a(network); a != INVALID; ++a)
        cout << network.id(a) << "(" << network.id(network.source(a)) << "," << network.id(network.target(a)) << "):\t" << flows[a] << endl;
}

void Network::printExistingFlow (SubstrateGraph& network, SubstrateGraph::ArcMap<Flow>& flows) {
    bool found = false;
    
    for (ArcIt a(network); a!= INVALID; ++a) {
        if (flows[a] > 0) {
            //print header
            if (!found) {
                cout << "Arc\t\t" << "Flow" << endl;
                found = true;
            }//if
            
            cout << network.id(a)
                 << "("    << network.id(network.source(a)) << "," << network.id(network.target(a))
                 << "):\t" << flows[a] << endl;;
        }//if
    }//for
    
    if (!found) {
        cout << "No flow!" << endl;
    }//if
}

void Network::printFlow(SubstrateGraph& network, vector<Arc>& arcs) {
    if (arcs.size() > 0) {
        cout << "Arc\t\t" << "Flow" << endl;
        for (size_t i = 0; i < arcs.size(); ++i) {
                cout << network.id(arcs[i])
                     << "("    << network.id(network.source(arcs[i])) << "," << network.id(network.target(arcs[i]))
                     << "):\t" << BANDWIDTH_UNIT    << endl;
        }//for
    }//if
    else {
        cout << "No flow!" << endl;
    }//if
}
