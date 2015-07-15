//
//  graph.h
//  ChainEmbedding
//
//  Created by Milad Ghaznavi on 3/19/15.
//  Copyright (c) 2015 Milad Ghaznavi, Nashid Shahriar. All rights reserved.
//

#pragma once

#include "basics.h"
#include <lemon/smart_graph.h>
#include <lemon/cost_scaling.h>
#include <lemon/dijkstra.h>
#include <lemon/lgf_reader.h>
#include <lemon/lgf_writer.h>

#define INVALID lemon::INVALID

typedef int               Capacity;
typedef int               Weight;
typedef lemon::Invalid    Invalid;

typedef lemon::SmartGraph Graph;
typedef Graph::Node       Node;
typedef Graph::NodeIt     NodeIt;
typedef Graph::Edge       Edge;
typedef Graph::EdgeIt     EdgeIt;
typedef Graph::Arc        Arc;
typedef Graph::ArcIt      ArcIt;

template<typename T>
using NodeMap = typename Graph::NodeMap<T>;

template<typename T>
using EdgeMap = typename Graph::EdgeMap<T>;

template<typename T>
using ArcMap = typename Graph::ArcMap<T>;

template<typename T>
using GraphReader = lemon::GraphReader<T>;

template<typename T>
using GraphWriter = lemon::GraphWriter<T>;

template<typename T>
using CostScaling = typename lemon::CostScaling<T>;

template<typename G, typename W>
using Dijkstra    = typename lemon::Dijkstra<G, W>;