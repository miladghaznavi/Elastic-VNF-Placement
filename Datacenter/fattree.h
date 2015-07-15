//
//  fattree.h
//  DCTopo
//
//  Created by Milad Ghaznavi on 4/9/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//
#pragma once

#include <sstream>
#include "basics.h"
#include "graph.h"

using std::stringstream;

class KAryFatTree : public virtual Graph {
#ifdef _GRAPH_IO_H_
friend class GraphIO;
#endif
    
public:
    #define DEFAULT_K 0
private:
    int  _k;
    void _build();

protected:
    void connectCoreLevelToAggLevel_();
    void connectAggLevelToEdgeLevel_();
    void connectEdgeLevelToHosts_();
    void init_(int k);
    
public:
    KAryFatTree (int k = DEFAULT_K);
    
    class NodeRangeIt : public Node {
        Identifier _end;
        const KAryFatTree* _graph;
        
    public:
        NodeRangeIt(Invalid i) : Node(i) { }
        
        NodeRangeIt(const KAryFatTree& graph, const Node& begin, const Node& end) :
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
    
    int k();
    int coreSwitchNum();
    int aggSwitchNum();
    int edgeSwitchNum();
    int hostNum();
    int podNum();
    int coreLinkNum();
    int aggLinkNum();
    int edgeLinkNum();
    
    Node firstHost();
    Node lastHost();
    
    NodeRangeIt coreSwitchIt();
    NodeRangeIt aggSwitchIt();
    NodeRangeIt edgeSwitchIt();
    NodeRangeIt hostIt();
    
    void copyTo(KAryFatTree&);
};
