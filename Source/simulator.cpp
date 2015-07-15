//
//  simulator.cpp
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 6/4/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "simulator.h"
#include <sstream>
#include <time.h>
using namespace std;

Simulator::Simulator(string path): _cngParser(path, true) {
    if (!_cngParser.parse()) {
        throw std::logic_error("Error to parse the config file!");
    }//if
}

void Simulator::_initCost() {
    _cngParser.select(PARAM_SEC);
    _params.timeSlotInSecond                    = atoi(_cngParser.get(PARAM_TIME_SLOT).c_str());
    _params.hostCostInCentsPerTimeSlot          = atof(_cngParser.get(PARAM_ENERGY   ).c_str());
    _params.bandwidthUnitCostInCentsPerTimeSlot = atof(_cngParser.get(PARAM_BNDWDH   ).c_str());
    _lambda                                     = atoi(_cngParser.get(PARAM_BALANCE  ).c_str());    
}

void Simulator::_initDatacenter() {
    _cngParser.select(DATACENTER_SEC);
    float bandwidthUnit = atof(_cngParser.get(BNDWDH_UNIT_MB).c_str());
    _params.bandwidthUnitInMB = bandwidthUnit;
    int k       = atoi(_cngParser.get(DATACENTER_K).c_str());
    string path = _cngParser.get(DATACENTER_PATH);
    
    // Topology
    _topology = new KAryFatTree(k);
    
    // Network
    SubstrateGraph sgraph;
    GraphIO::readGraph(sgraph, path);
    _network = new SubstrateGraph();
    sgraph.copyTo(*_network);
    
    Network::normalizeArcCap(*_network, bandwidthUnit);
}

void Simulator::_initVNF() {
    float bandwidthUnit = atof(_cngParser.get(BNDWDH_UNIT_MB).c_str());
  
    _cngParser.select(VNF_SEC);
    _params.vnfCPU                           = atoi(_cngParser.get(VNF_CPU).c_str());
    _params.vnfMaxCapInTermsOfDemandsNum     = atoi(_cngParser.get(VNF_CAP).c_str());
    _params.vnfRamSizeInTermsOfBandwidthUnit = ceil(atof(_cngParser.get(VNF_RAM).c_str()) / bandwidthUnit);
    
    //Normalize node capacity
    Network::normalizeNodeCap(*_network, _params.vnfCPU);
}

void Simulator::_initDemands() {
    _cngParser.select(DEMANDS_SEC);
    string path = _cngParser.get(DEMANDS_PATH);
    _events.init(path);
    _events.parse();
}

void Simulator::_initOutput() {
    _cngParser.select(OUTPUT_SEC);
    _outputPath = _cngParser.get(OUTPUT_PATH);
    stringstream ss(_cngParser.get(OUTPUT_REP_FLOW));
    ss >> std::boolalpha >> _reportFlow;
}

Time Simulator::_exactGapToNextEvent(Event& curr) {
    Event next = _events.top();
    Time currTime = (curr.type == Event::Arrival) ? curr.demand.arrival() : curr.demand.departure();
    Time re = ((next.type == Event::Arrival) ? next.demand.arrival() : next.demand.departure())  - currTime;
    return re;
}

void Simulator::init() {
    // Init variables
    using std::cout;
    using std::endl;
    cout << "\nInit costs..." << endl;
    _initCost();
    cout << "Init datacenter..." << endl;
    _initDatacenter();
    cout << "Init VNF..." << endl;
    _initVNF();
    cout << "Init demands..." << endl;
    _initDemands();
    cout << "Init output..." << endl;
    _initOutput();
}

void Simulator::createSample(string path) {
    // Save current data and clear
    INIParser tmp(path, true);
    
    /* Parameters */
    tmp.create(PARAM_SEC);
    tmp.select(PARAM_SEC);
    tmp.set   (PARAM_TIME_SLOT, 5 * 60);
    tmp.set   (PARAM_ENERGY   , 6);
    tmp.set   (PARAM_BNDWDH   , 1);
    tmp.set   (PARAM_BALANCE  , 4);
    
    /* Datacenter */
    tmp.create(DATACENTER_SEC);
    tmp.select(DATACENTER_SEC);
    tmp.set   (BNDWDH_UNIT_MB , 20);
    tmp.set   (DATACENTER_K   , 16);
    tmp.set   (DATACENTER_PATH, "datacenter");
    
    /* VNF */
    tmp.create(VNF_SEC);
    tmp.select(VNF_SEC);
    tmp.set   (VNF_CAP, 5);
    tmp.set   (VNF_CPU, 1);
    tmp.set   (VNF_RAM, (3.7 * 8 * 1024) / 20);
    
    /* Demands */
    tmp.create(DEMANDS_SEC);
    tmp.select(DEMANDS_SEC);
    tmp.set   (DEMANDS_PATH, "demands");

    /* Output */
    tmp.create(OUTPUT_SEC);
    tmp.select(OUTPUT_SEC);
    tmp.set   (OUTPUT_PATH    , "output");
    tmp.set   (OUTPUT_REP_FLOW, "true");
    
    //Save to file
    tmp.save();
}

void Simulator::start() {
    setAlg_    ();
    setAlgName_();
    Report R(_outputPath, _algName, _reportFlow);
    
    int count = 0;
    
    while (!_events.empty()) {
        //Next event
        Event e = _events.next();
        Report::EventProcessResult processResult;
        count ++;
        
        cout << "Processing event " << count - 1  << "(" << e.demand.source() << "," << e.demand.target() << "): " << ((e.type == Event::Arrival) ? ARRIVAL : DEPARTURE) << endl;
        
        // Run algorithm and collect processing time
        clock_t processTime = clock();
        if (e.type == Event::Arrival) {
            if (_algorithm->arrival(e.demand, _lambda)) {
                processResult = Report::EventProcessResult::ArrivalAccepted;
            }//if
            else {
                processResult = Report::EventProcessResult::ArrivalRejected;
            }//else
        }//if
        else {
            auto found = _rejected.find(e.identifier());
            if (found == _rejected.end()) {
                _algorithm->departure(e.demand, _lambda);
                processResult = Report::EventProcessResult::DepartureAccepted;
            }//if
            else {
                processResult = Report::EventProcessResult::DepartureRejected;
            }//else
        }//else
        
        // Report
        if (processResult == Report::EventProcessResult::ArrivalRejected ||
            processResult == Report::EventProcessResult::DepartureRejected) {
            _rejected.insert(std::make_pair(e.identifier(), e));
        }//else
        
        Time repProcessTime = (clock() - processTime) / (double) CLOCKS_PER_SEC;
        Time exactGap = _exactGapToNextEvent(e);
        R.report(e, *_algorithm, exactGap, repProcessTime, processResult);
        
        cout.flush();
    }//while
}

void Simulator::getDaracenter(SubstrateGraph & network, KAryFatTree& topology) {
    _network ->copyTo(network);
    _topology->copyTo(topology);
}

void Simulator::getParameters(Parameters& params) {
    params = _params;
    
}

Simulator::~Simulator() {
    _topology = nullptr;
    _network  = nullptr;
}
