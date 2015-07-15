//
//  report.cpp
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 6/6/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "report.h"
#include <algorithm>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
using namespace std;

Report::Report(string path, string name, bool reportFlows) :
    _path        (_initPath(path)),
    _name        (_initName(name)),
    _costFile    (_costPath().c_str()),
    _utilFile    (_utilPath().c_str()),
    _timeFile    (_timePath().c_str()),
    _acpRatioFile(_accRatioPath().c_str()),
    _rejectedFile(_rejectedPath().c_str()),
    _reportFlows (_initReportFlows(reportFlows)) {
    
    _reportCount = 0;
    _rejectedCount = 0;
    _arriveAcceptedCount = 0;
    _departAcceptedCount = 0;
    _writeHeaders();
}

void Report::_writeHeaders() {
    _costFile << COST_HEADER << endl;
    _utilFile << UTIL_HEADER << endl;
    _timeFile << TIME_HEADER << endl;
    _acpRatioFile << ACCEPTANCE_HEADER << endl;
    _rejectedFile << REJECTED_HEADER << endl;
}

bool Report::_dirExists(string path) {
    bool re = true;
    struct stat info;
    if(stat(path.c_str(), &info) != 0)
        re = false;
    else if(info.st_mode & S_IFDIR)
        re = true;
    else
        re = false;
    return re;
}

string Report::_initPath(string path) {
    if (!_dirExists(path)) {
        stringstream ss;
        ss << "Directory '" << path << "' does not exist!";
        throw invalid_argument(ss.str());
    }//if
    return path;
}

string Report::_initName(string name) {
    stringstream ss;
    ss << _path << "/" << name;
    mkdir(ss.str().c_str(), 0777);
    return name;
}

bool Report::_initReportFlows(bool reportFlows) {
    if (reportFlows) {
        stringstream ss;
        ss << _path << "/" << _name << "/" << FLOWS_FOLDER;
        mkdir(ss.str().c_str(), REPORT_FILE_PER);
    }//if
    return reportFlows;
}

string Report::_costPath() {
    stringstream ss;
    ss << _path << "/" << _name << "/" << COST_FILE;
    return ss.str();
}

string Report::_utilPath() {
    stringstream ss;
    ss << _path << "/" << _name << "/" << UTIL_FILE;
    return ss.str();
}

string Report::_timePath() {
    stringstream ss;
    ss << _path << "/" << _name << "/" << TIME_FILE;
    return ss.str();
}

string Report::_accRatioPath() {
    stringstream ss;
    ss << _path << "/" << _name << "/" << ACCEPTANCE_FILE;
    return ss.str();
}

string Report::_rejectedPath() {
    stringstream ss;
    ss << _path << "/" << _name << "/" << REJECTED_FILE;
    return ss.str();
}

template<typename T>
double Report::_linkUtilization(Algorithm& alg, T e) {
    double re = 0;
    int count = 0;
    for (; e != INVALID; ++e) {
        Identifier eId = alg.network_.id(e);
        
        Arc a1 = alg.network_.arcFromId(2 * eId);
        Arc a2 = alg.network_.arcFromId(2 * eId + 1);
        if (alg.network_.capacity(a1) > 0) {
            re += alg.network_.utilization(a1);
            count ++;
        }//if
        if (alg.network_.capacity(a2) > 0) {
            re += alg.network_.utilization(a2);
            count ++;
        }//if
    }//for
    return (alg.network_.edgeNum() > 0) ? re / count : re;
}

template<typename T>
double Report::_usedLinkUtilization(Algorithm& alg, T e) {
    double re = 0;
    int count = 0;
    for (; e != INVALID; ++e) {
        Identifier eId = alg.network_.id(e);
        
        Arc a1 = alg.network_.arcFromId(2 * eId);
        Arc a2 = alg.network_.arcFromId(2 * eId + 1);
        if (alg.network_.capacity(a1) > 0 && alg.network_.allocated(a1) > 0) {
            re += alg.network_.utilization(a1);
            count ++;
        }//if
        if (alg.network_.allocated(a2) > 0 && alg.network_.allocated(a2) > 0) {
            re += alg.network_.utilization(a2);
            count ++;
        }//if
    }//for
    return (count > 0) ? (re / count) : re;

}

double Report::_usedHostUtilization(Algorithm& alg) {
    double re = 0;
    int count = 0;
    for (auto h = alg.topology_.hostIt(); h != INVALID; ++h) {
        Capacity cap = alg.network_.capacity(h);
        Capacity alc = alg.network_.allocated(h);
        
        if (cap > 0 && alc > 0) {
            count++;
            re += ((double)alc) / cap;
        }//if
    }//for
    
    return (count > 0) ? re / count : re;
}

double Report::_usedLinkUtilization(Algorithm& alg) {
    return _usedLinkUtilization(alg, EdgeIt(alg.network_));
}

double Report::_hostUtilization(Algorithm& alg) {
    Capacity count = 0;
    for (auto h = alg.topology_.hostIt(); h != INVALID; ++h)
        count += alg.network_.capacity(h);
    
    return (count > 0) ? ((double) alg.facilityNum()) / count : 0;
}

double Report::_linkUtilization(Algorithm& alg) {
    return _linkUtilization(alg, EdgeIt(alg.network_));
}

double Report::_coreLinkUtilization(Algorithm& alg) {
    return _linkUtilization(alg, alg.topology_.coreLinkIt());
}

double Report::_aggLinkUtilization(Algorithm& alg) {
    return _linkUtilization(alg, alg.topology_.aggLinkIt());
}

double Report::_edgeLinkUtilization(Algorithm& alg) {
    return _linkUtilization(alg, alg.topology_.edgeLinkIt());
}

double Report::_usedCoreLinkUtilization(Algorithm& alg) {
    return _usedLinkUtilization(alg, alg.topology_.coreLinkIt());
}

double Report::_usedAggLinkUtilization (Algorithm& alg) {
    return _usedLinkUtilization(alg, alg.topology_.aggLinkIt());
}

double Report::_usedEdgeLinkUtilization(Algorithm& alg) {
    return _usedLinkUtilization(alg, alg.topology_.edgeLinkIt());
}

double Report::_vnfResourcesUtilization(Algorithm& alg) {
    double result = 0;
    if (alg.facilityNum() > 0) {
        result = ((double)alg.demandNum()) / (alg.facilityNum() * alg.params_.vnfMaxCapInTermsOfDemandsNum);
    }//if
    return result;
}

void Report::_reportCost(Event& e, Algorithm& alg, Time& exactGapToNextEvent, EventProcessResult& processResult) {
    string type = (e.type == Event::Arrival) ? ARRIVAL : DEPARTURE;
    double time = (e.type == Event::Arrival) ? e.demand.arrival() : e.demand.departure();
    bool succeed = processResult == EventProcessResult::ArrivalAccepted ||
                    processResult == EventProcessResult::DepartureAccepted;
    string result = (succeed) ? ACCEPTED : REJECTED;
    // Costs
    Money iCost = alg.installationCost  (exactGapToNextEvent);
    Money tCost = alg.transportationCost(exactGapToNextEvent);
    Money mCost = (succeed) ? alg.migrationCost   () : NO_COST;
    Money rCost = (succeed) ? alg.reassignmentCost() : NO_COST;
    Money totalCost = iCost + tCost + mCost + rCost;
    
    // Report
    _costFile <<
        e.identifier()        << DELIM <<
        type                  << DELIM <<
        result                << DELIM <<
        time                  << DELIM <<
        alg.demandNum()       << DELIM <<
        alg.facilityNum()     << DELIM <<
        alg.migrationNum()    << DELIM <<
        alg.reassignmentNum() << DELIM <<
        iCost                 << DELIM <<
        tCost                 << DELIM <<
        mCost                 << DELIM <<
        rCost                 << DELIM <<
        totalCost             << endl;
    _costFile.flush();
}

void Report::_reportUtil(Event& e, Algorithm& alg, EventProcessResult& processResult) {
    string type = (e.type == Event::Arrival) ? ARRIVAL : DEPARTURE;
    double time = (e.type == Event::Arrival) ? e.demand.arrival() : e.demand.departure();
    bool succeed = processResult == EventProcessResult::ArrivalAccepted ||
                    processResult == EventProcessResult::DepartureAccepted;
    string result = (succeed) ? ACCEPTED : REJECTED;
    
    _utilFile <<
        e.identifier()                << DELIM <<
        type                          << DELIM <<
        result                        << DELIM <<
        time                          << DELIM <<
        alg.demandNum()               << DELIM <<
        _usedHostUtilization(alg)     << DELIM <<
        _hostUtilization(alg)         << DELIM <<
        _usedLinkUtilization(alg)     << DELIM <<
        _linkUtilization(alg)         << DELIM <<
        _usedCoreLinkUtilization(alg) << DELIM <<
        _coreLinkUtilization(alg)     << DELIM <<
        _usedAggLinkUtilization(alg)  << DELIM <<
        _aggLinkUtilization(alg)      << DELIM <<
        _usedEdgeLinkUtilization(alg) << DELIM <<
        _edgeLinkUtilization(alg)     << DELIM <<
        _vnfResourcesUtilization(alg) << endl;
    
    _utilFile.flush();
}

void Report::_reportTime(Event& e, Algorithm& alg, Time& processTime, EventProcessResult& processResult) {
    string type = (e.type == Event::Arrival) ? ARRIVAL : DEPARTURE;
    double time = (e.type == Event::Arrival) ? e.demand.arrival() : e.demand.departure();
    bool succeed = processResult == EventProcessResult::ArrivalAccepted ||
                    processResult == EventProcessResult::DepartureAccepted;
    string result = (succeed) ? ACCEPTED : REJECTED;
    
    _timeFile <<
        e.identifier()  << DELIM <<
        type            << DELIM <<
        result          << DELIM <<
        time            << DELIM <<
        alg.demandNum() << DELIM <<
        processTime     << endl;
    
    _timeFile.flush();
}

void Report::_reportAccp(Event& e, Algorithm& alg, EventProcessResult& processResult) {
    string type = (e.type == Event::Arrival) ? ARRIVAL : DEPARTURE;
    bool succeed = processResult == EventProcessResult::ArrivalAccepted ||
                    processResult == EventProcessResult::DepartureAccepted;
    double time = (e.type == Event::Arrival) ? e.demand.arrival() : e.demand.departure();
    string result = (succeed) ? ACCEPTED : REJECTED;
    
    _acpRatioFile <<
        e.identifier()                  << DELIM <<
        type                            << DELIM <<
        result                          << DELIM <<
        time                            << DELIM <<
        alg.demandNum()                 << DELIM <<
        // Number of accepted arrival events
        _arriveAcceptedCount            << DELIM <<
        // Number of accepted departure events
        _departAcceptedCount            << DELIM <<
        // Total number of accpeted events
        _arriveAcceptedCount + _departAcceptedCount << DELIM <<
        // Number of rejected demands
        _rejectedCount                  << DELIM <<
        // Arrival acceptance ratio
        ((double)_arriveAcceptedCount) / (_arriveAcceptedCount + _rejectedCount) << DELIM <<
        // Total acceptance ratio
        ((double)_arriveAcceptedCount + _departAcceptedCount) / (_arriveAcceptedCount + _departAcceptedCount + _rejectedCount) << endl;
    
    _acpRatioFile.flush();
}

void Report::_reportFlow(Event& e, Algorithm& alg) {
    string type = (e.type == Event::Arrival) ? ARRIVAL : DEPARTURE;
    stringstream path;
    path << _path << "/" << _name << "/" << FLOWS_FOLDER << "/" << _reportCount << "-" << type;
    
    SubstrateGraph::EdgeMap<Flow> edgeFlow(alg.network_);
    for (EdgeIt e(alg.network_); e != INVALID; ++e) {
        Identifier eId = alg.network_.id(e);
        Arc a1 = alg.network_.arcFromId(eId * 2);
        Arc a2 = alg.network_.arcFromId(eId * 2 + 1);
        
        Capacity al1 = alg.network_.allocated(a1);
        Capacity al2 = alg.network_.allocated(a2);
        
        // We would have the same amount of flow in one of the arcs
        edgeFlow[e] = (al1 > 0) ? al1 : al2;
    }//for
    
    ofstream os(path.str().c_str());
        GraphWriter<SubstrateGraph>(alg.network_, os).
        nodeMap(NODE_RES , alg.network_._nodeResidual).
        edgeMap(EDGE_FLOW, edgeFlow).
        run();
        os.close();
}

void Report::_reportRejected(Event& e) {
    string type = (e.type == Event::Arrival) ? ARRIVAL : DEPARTURE;
    double time = (e.type == Event::Arrival) ? e.demand.arrival() : e.demand.departure();
    
    _rejectedFile <<
        e.identifier()      << DELIM <<
        type                << DELIM <<
        time                << DELIM <<
        e.demand.source()   << DELIM <<
        e.demand.target()   << DELIM <<
        e.demand.arrival()  << DELIM <<
        e.demand.duration() << endl;
    
    _rejectedFile.flush();
}

void Report::report(Event& e, Algorithm& alg, Time& exactGapToNextEvent, Time& processTime, EventProcessResult processResult) {
    _reportCount ++;
    
    switch(processResult) {
        case Report::EventProcessResult::ArrivalAccepted:
            _arriveAcceptedCount ++;
            break;
        case Report::EventProcessResult::ArrivalRejected:
            _reportRejected(e);
            _rejectedCount ++;
            break;
        case Report::EventProcessResult::DepartureAccepted:
            _departAcceptedCount ++;
            break;
        case Report::EventProcessResult::DepartureRejected:
            break;
    }//switch
    
    _reportCost(e, alg, exactGapToNextEvent, processResult);
    _reportUtil(e, alg, processResult);
    _reportTime(e, alg, processTime, processResult);
    _reportAccp(e, alg, processResult);
    
    if (_reportFlows)
        _reportFlow(e, alg);
}

Report::~Report() {
    try {
        _costFile.close();
        _utilFile.close();
        _timeFile.close();
        _rejectedFile.close();
        _acpRatioFile.close();
    }//try
    catch(...){ }//catch
}