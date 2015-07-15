//
//  simulator.h
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 6/4/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#pragma once

#include <unordered_map>
#include "basics.h"
#include "INI.h"
#include "event.h"
#include "report.h"

using std::string;
using std::ofstream;
using std::vector;

#define DATACENTER_SEC  "datacenter"
#define BNDWDH_UNIT_MB  "bandwidth-unit"
#define DATACENTER_K    "k"
#define DATACENTER_PATH "path"

#define VNF_SEC         "vnf"
#define VNF_CAP         "cap"
#define VNF_CPU         "cpu"
#define VNF_RAM         "ram"

#define PARAM_SEC       "parameters"
#define PARAM_TIME_SLOT "time-slot"
#define PARAM_ENERGY    "energy"
#define PARAM_BNDWDH    "bandwidth"
#define PARAM_BALANCE   "balance"

#define DEMANDS_SEC     "demands"
#define DEMANDS_PATH    "path"

#define OUTPUT_SEC      "output"
#define OUTPUT_PATH     "path"
#define OUTPUT_REP_FLOW "report-flow-files"

typedef INI<> INIParser;

class Simulator : public BaseClass {
public:
    INIParser    _cngParser;
    
    /* Parameters */
    Parameters   _params;
    double       _lambda;
    
    /* Datacenter */
    KAryFatTree* _topology;
    SubstrateGraph*   _network;
    
    /*  Demands   */
    EventQueue _events;
    unordered_map<Identifier, Event> _rejected;
    
    /*  Output    */
    string     _outputPath;
    bool       _reportFlow;
    
    /* Algorithms */
    Algorithm* _algorithm;
    string     _algName;
    
    /*   Methods  */
    void _initVNF       ();
    void _initCost      ();
    void _initDatacenter();
    void _initDemands   ();
    void _initOutput    ();
    Time _exactGapToNextEvent(Event& curr);
    
protected:
    virtual void setAlg_    () = 0;
    virtual void setAlgName_() = 0;
    
public:
    Simulator(string);
    
    void init ();
    void start();
    
    void getDaracenter(SubstrateGraph &, KAryFatTree&);
    void getParameters(Parameters&);
    
    static void createSample(string);
    ~Simulator();
};