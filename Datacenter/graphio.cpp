//
//  graphio.cpp
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 5/5/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "graphio.h"
#include <fstream>

void GraphIO::readGraph(KAryFatTree& g, istream& is) {
    GraphReader<KAryFatTree> reader(g, is);
    reader.attribute(K_ATTR, g._k).run();
}

void GraphIO::readGraph(KAryFatTree& g, string path) {
    std::ifstream input(path.c_str());
    readGraph(g, input);
    input.close();
}

void GraphIO::readGraph(SubstrateGraph& g, istream& is){
    GraphReader<SubstrateGraph>(g, is).
    nodeMap(NODE_CAP, g._nodeCapacity).
    nodeMap(NODE_RES, g._nodeResidual).
    edgeMap(EDGE_CAP, g._edgeCapacity).
    edgeMap(EDGE_RES, g._edgeResidual).
    edgeMap(EDGE_WEI, g._edgeWeight).
        run();
}

void GraphIO::readGraph(SubstrateGraph& g, string path){
    std::ifstream input(path.c_str());
    readGraph(g, input);
    input.close();
}

//void GraphIO::readGraph(FlowGraph& g, istream& is) {
//    GraphReader<FlowGraph>(g, is).
//        nodeMap(NODE_CAP, g._nodeCapacity).
//        //    nodeMap(NODE_ALL, g._nodeAllocated).
//        nodeMap(NODE_SUP, g.supply_).
//        nodeMap(NODE_DEM, g.demandId_).
//        nodeMap(NODE_IS_KARIZ, g.isKariz_).
//        edgeMap(EDGE_CAP, g._edgeCapacity).
//        //    edgeMap(EDGE_ALL, g._edgeAllocated).
//        edgeMap(EDGE_WEI, g._edgeWeight).
////        edgeMap(EDGE_FLW, g._flow).
//        run();
//}
//
//void GraphIO::readGraph(FlowGraph& g, string path) {
//    std::ifstream input(path.c_str());
//    readGraph(g, input);
//    input.close();
//}

void GraphIO::writeGraph(KAryFatTree& g, ostream& os) {
    GraphWriter<KAryFatTree> writer(g, os);
    writer.attribute(K_ATTR, g.k()).run();
}

void GraphIO::writeGraph(KAryFatTree& g, string path) {
    std::ofstream output(path.c_str());
    writeGraph(g, output);
    output.close();
}

void GraphIO::writeGraph(SubstrateGraph& g, ostream& os){
    GraphWriter<SubstrateGraph>(g, os).
        nodeMap(NODE_CAP, g._nodeCapacity).
        nodeMap(NODE_RES, g._nodeResidual).
        edgeMap(EDGE_CAP, g._edgeCapacity).
        edgeMap(EDGE_RES, g._edgeResidual).
        edgeMap(EDGE_WEI, g._edgeWeight).
        run();
}

void GraphIO::writeGraph(SubstrateGraph& g, string path){
    std::ofstream output(path.c_str());
    writeGraph(g, output);
    output.close();
}

//void GraphIO::writeGraph(FlowGraph& g, ostream& os){
//    GraphWriter<FlowGraph>(g, os).
//        nodeMap(NODE_CAP, g._nodeCapacity).
//        //    nodeMap(NODE_ALL, g._nodeAllocated).
//        nodeMap(NODE_SUP, g.supply_).
//        nodeMap(NODE_DEM, g.demandId_).
//        edgeMap(EDGE_CAP, g._edgeCapacity).
//        //    edgeMap(EDGE_ALL, g._edgeAllocated).
//        edgeMap(EDGE_WEI, g._edgeWeight).
////        edgeMap(EDGE_FLW, g._flow).
//        run();
//}
//
//void GraphIO::writeGraph(FlowGraph& g, string path){
//    std::ofstream output(path.c_str());
//    writeGraph(g, output);
//    output.close();
//}
