//
//  substrategraph.h
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 5/2/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#pragma once

#include <stdexcept>
#include <sstream>
#include <math.h>
#include "basics.h"
#include "resource.h"
#include "graph.h"

class SubstrateGraph : public virtual Graph {
#ifdef _GRAPH_IO_H_
friend class GraphIO;
#endif
public:
#define NO_CAPACITY     0
#define NO_WEIGHT       0
#define INFINITE_WEIGHT INFINITY
    typedef NodeMap<Capacity> NodeCapacityMap;
    typedef ArcMap <Capacity> ArcCapacityMap;
    typedef EdgeMap<Capacity> EdgeCapacityMap;
    typedef ArcMap <Weight>   ArcWeight;
    typedef EdgeMap<Weight>   EdgeWeight;
    
    NodeCapacityMap _nodeCapacity;
    NodeCapacityMap _nodeResidual;
    EdgeCapacityMap _edgeCapacity;
    ArcCapacityMap  _arcCapacity;
    EdgeCapacityMap _edgeResidual;
    ArcCapacityMap  _arcResidual;
    EdgeWeight      _edgeWeight;
    ArcWeight       _arcWeight;
    
public:
    SubstrateGraph();
    SubstrateGraph(Graph&);
    
    virtual Capacity capacity(Node n);
    virtual Capacity capacity(Edge e);
    virtual Capacity capacity(Arc  a);
    virtual Weight   weight  (Edge e);
    virtual Weight   weight  (Arc  a);
    virtual Capacity residual(Node n);
    virtual Capacity residual(Edge e);
    virtual Capacity residual(Arc a);
    virtual Capacity allocated(Node n);
    virtual Capacity allocated(Edge e);
    virtual Capacity allocated(Arc a);
    virtual void     capacity(Node n, Capacity amount);
    virtual void     capacity(Edge e, Capacity amount);
    virtual void     capacity(Arc  a, Capacity amount);
    virtual void     weight  (Edge e, Weight w);
    virtual void     weight  (Arc  a, Weight w);
    virtual void     allocate(Node n, Capacity amount);
    virtual void     allocate(Edge e, Capacity amount);
    virtual void     allocate(Arc  a, Capacity amount);
    virtual void     release (Node n, Capacity release);
    virtual void     release (Edge e, Capacity release);
    virtual void     release (Arc  a, Capacity release);
    virtual void     allocateAll(Node n);
    virtual void     allocateAll(Edge e);
    virtual void     allocateAll(Arc  a);
    virtual void     releaseAll (Node n);
    virtual void     releaseAll (Edge e);
    virtual void     releaseAll (Arc  a);
    virtual double   utilization(Node n);
    virtual double   utilization(Edge e);
    virtual double   utilization(Arc  a);
    virtual double   nodesUtilization();
    virtual double   edgesUtilization();
    virtual double   arcsUtilization ();
    virtual void     copyTo(SubstrateGraph& to);
    virtual void     copyFrom(Graph& from);
};