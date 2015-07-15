//
//  graphio.h
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 5/5/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#pragma once

#ifndef _GRAPH_IO_H_
#define _GRAPH_IO_H_
#endif

#include "substrategraph.h"
#include "flowgraph.h"
#include "fattree.h"
#include "vl2.h"
#include <string>
#include <iostream>

using std::string;
using std::istream;
using std::ostream;

#define K_ATTR          "k"
#define NODE_CAP        "capacity"
#define EDGE_CAP        "capacity"
#define NODE_RES        "residual"
#define NODE_ALC        "allocated"
#define EDGE_RES        "residual"
#define EDGE_WEI        "weight"
#define NODE_DEM        "demandId"
#define NODE_SUP        "supply"
#define EDGE_FLW        "flow"

class GraphIO {
public:
    static void readGraph(KAryFatTree&    g, istream& is);
    static void readGraph(KAryFatTree&    g, string path);
    static void readGraph(SubstrateGraph& g, istream& is);
    static void readGraph(SubstrateGraph& g, string path);
//    static void readGraph(FlowGraph&      g, istream& is);
//    static void readGraph(FlowGraph&      g, string path);
    
    static void writeGraph(KAryFatTree&    g, ostream& os);
    static void writeGraph(KAryFatTree&    g, string path);
    static void writeGraph(SubstrateGraph& g, ostream& os);
    static void writeGraph(SubstrateGraph& g, string path);
//    static void writeGraph(FlowGraph&      g, ostream& os);
//    static void writeGraph(FlowGraph&      g, string path);
};
