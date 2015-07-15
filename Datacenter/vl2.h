//
//  vl2.h
//  DCTopo
//
//  Created by Milad Ghaznavi on 4/14/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#pragma once

#include <stdexcept>
#include <iostream>
#include "graph.h"

/*
    The implementation of this class represents microtosft Virtual Lauer 2 (VL2) topology.
    For more information refer to:
    Greenberg, Albert, et al. "VL2: a scalable and flexible data center network." 
    ACM SIGCOMM computer communication review. Vol. 39. No. 4. ACM, 2009.
 */

class VirtualLayer2 : public virtual Graph {
#ifdef _GRAPH_IO_H_
    friend class GraphIO;
#endif

public:
    #define DEFAULT_DI  0
    #define DEFAULT_DA  0
    #define DEFAULT_DDT 0

private:
    int _di;
    int _da;
    int _ddt;
    void _build();

protected:
    Identifier firstIntermedSwitch_();
    Identifier firstAggSwitch_     ();
    Identifier firstToRSwitch_     ();
    Identifier firstHost_          ();
    Identifier lastIntermedSwitch_ ();
    Identifier lastAggSwitch_      ();
    Identifier lastToRSwitch_      ();
    Identifier lastHost_           ();
    
public:
    VirtualLayer2 (int di = DEFAULT_DI, int da = DEFAULT_DA, int ddt = DEFAULT_DDT);
    
    int  di ();
    int  da ();
    int  ddt();
    int  intermedSwitchNum();
    int  aggSwitchNum     ();
    int  ToRSwitchNum     ();
    int  hostNum          ();

    class NodeRangeIt : public Node {
        Identifier _end;
        const VirtualLayer2* _graph;
        
    public:
        NodeRangeIt(Invalid i) : Node(i) { }
        
        NodeRangeIt(const VirtualLayer2& graph, const Node& begin, const Node& end) :
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
    
    NodeRangeIt intermedSwitchIt();
    NodeRangeIt aggSwitchIt();
    NodeRangeIt ToRSwitchIt();
    NodeRangeIt hostIt();
};