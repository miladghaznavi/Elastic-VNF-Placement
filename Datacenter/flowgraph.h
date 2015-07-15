//
//  flowgraph.h
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 5/3/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#pragma once

#include "demand.h"
#include "facility.h"
#include "substrategraph.h"

#define IS_KARIZ    "isKariz"
#define IS_FAC_INS  "facilityInstalled"
#define EDGE_FLOW   "flow"

typedef int Supply;
typedef int Flow;

// The arrangement of nodes must be in this way to make sure that the class works properly
/*
 |_______________________________________||___________________||_ _ _ _ _ _ _ _|
    Ordinary nodes (switches and hosts)       facility nodes      kariz nodes
 We assume that hosts are arranged in a continues sequence; no other nodes are between.
 */
class FlowGraph: public SubstrateGraph {
#ifdef _GRAPH_IO_H_
friend class GraphIO;
#endif
public:
#define BANDWIDTH_UNIT    1
#define KARIZ_DEMAND_VAL  2
#define NO_SUPPLY         0
#define NO_FLOW           0
#define NOT_KARIZ         false
#define NO_FACILITY       false
    typedef NodeMap<bool>          IsKariz;
    typedef NodeMap<bool>          IsFacInstalled;
    typedef NodeMap<Weight>        DistMap;
    typedef NodeMap<Supply>        SupplyMap;
    typedef NodeMap<Identifier>    NodeIdMap;
    typedef ArcMap <Capacity>      FlowMap;
    typedef CostScaling<FlowGraph> MinCostFlow;
    
//protected:
    // We keep supply to zero for every node, other than it is externally set by child class
    SupplyMap       supply_;
    IsFacInstalled  isFacInstalled_;
    IsKariz         isKariz_;
    NodeIdMap       demandId_;
    FlowMap         flow_;
    bool            facilityGenerated_;
    Node            lastOrdinaryNode_;
    Node            lastFacilityNode_;
    
private:
    Node _demandKariz   (Demand, Edge&, Edge&, Identifier);
    void _invalidateNode(Node&);
    void _invalidateEdge(Edge&);
    void _invalidateArc (Arc &);
    
public:
    class NodeRangeIt : public Node {
        Identifier _end;
        const FlowGraph* _graph;
        
    public:
        NodeRangeIt(Invalid i) : Node(i) { }
        
        NodeRangeIt(const FlowGraph& graph, const Node& begin, const Node& end) :
        _graph(&graph), Node(begin) {
            this->_end = _graph->id(end);
        }
        
        NodeRangeIt& operator++() {
            //Save _end pointer
            auto end = _end;
            _graph->next(*this);
            //restor _end pointer
            this->_end = end;
            
            if (_end == _graph->id(*this)) {
                *this = Invalid();
            }//if
            return *this;
        }
    };
    
    FlowGraph();
    FlowGraph(Graph& graph);
    
    void   generateFacilityNodes (Node, Node, Facility);
    bool   facilityNodesGenerated();
    // Requirement: Before calling this function, generateFacilityNodes must be called;
    void        addDemand     (Demand&);
    void        removeDemand  (Demand&);
    void        copyTo        (FlowGraph&);
    NodeRangeIt facilityNodeIt();
    Supply      supply        (Node);
    void        supply        (Node, Supply);

    class KarizIt : public Node {
        const FlowGraph* _graph;
    public:
        KarizIt(Invalid i) : Node(i) { }
        KarizIt(const FlowGraph& graph) :
        _graph(&graph), Node(graph.nodeFromId(graph.nodeNum() - 1)) {
            if (_graph->lastFacilityNode_ != INVALID)
                throw std::logic_error("There is no facility nodes! Therefore, we cannot keep track of Karizs!");
            if (*this == _graph->lastFacilityNode_)
                *this = Invalid();
        }
        
        KarizIt& operator++() {
            do {
                _graph->next(*this);
                if (_graph->demandId_[*this] != INVALID_ID) {
                    break;
                }//if
                if (_graph->lastFacilityNode_ == *this) {
                    *this = Invalid();
                    break;
                }//if
            } while(*this != INVALID);
            return *this;
        }
    };

};
