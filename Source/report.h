//
//  report.h
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 6/6/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#pragma once

#ifndef _REPORT_H_
#define _REPORT_H_
#endif

#include "algorithm.h"
#include "fattree.h"
#include "simulator.h"
#include "graphio.h"
#include "event.h"

// Files
#define COST_FILE       "cost.txt"
#define UTIL_FILE       "util.txt"
#define TIME_FILE       "time.txt"
#define ACCEPTANCE_FILE "acceptance.txt"
#define REJECTED_FILE   "rejected.txt"
#define REPORT_FILE_PER 0777

//HEADERS
#define COST_HEADER       "id,type,result,time,demands,vnfs,migrations,reassigns,installCost,transCost,migCost,reasgnCost,totalCost"
#define UTIL_HEADER       "id,type,result,time,demands,usedHostUtil,allHostUtil,usedLinksUtil,allLinksUtil,usedCoreLinksUtil,allCoreLinksUtil,usedAggLinksUtil,allAggLinksUtil,usedEdgeLinksUtil,allEdgeLinksUtil,facilityUtil"
#define ACCEPTANCE_HEADER "id,type,result,time,demands,accArr,accDep,acc,rejected,arrAccRatio,totalAccRatio"
#define REJECTED_HEADER   "id,type,time,src,trg,arr,dep"
#define TIME_HEADER       "id,type,result,time,demands,processTime"

// Values
#define FLOWS_FOLDER    "flows"
#define ARRIVAL         "arrive"
#define DEPARTURE       "depart"
#define ACCEPTED        "accepted"
#define REJECTED        "rejected"

// Delimeter
#define DELIM           ','

using std::ofstream;

class Report {
public:
    enum EventProcessResult {
        ArrivalAccepted,
        ArrivalRejected,
        DepartureAccepted,
        DepartureRejected
    };
private:
    string   _path;
    string   _name;
    ofstream _costFile;
    ofstream _utilFile;
    ofstream _timeFile;
    ofstream _acpRatioFile;
    ofstream _rejectedFile;
    bool     _reportFlows;
    size_t   _reportCount;
    size_t   _arriveAcceptedCount;
    size_t   _departAcceptedCount;
    size_t   _rejectedCount;
    
    bool   _dirExists(string);
    string _initPath (string);
    string _initName (string);
    string _costPath ();
    string _utilPath ();
    string _timePath ();
    string _accRatioPath();
    string _rejectedPath();
    bool   _initReportFlows(bool);
    
    template<typename T>
    double _usedLinkUtilization    (Algorithm&, T);
    template<typename T>
    double _linkUtilization        (Algorithm&, T);
    double _hostUtilization        (Algorithm&);
    double _linkUtilization        (Algorithm&);
    double _usedHostUtilization    (Algorithm&);
    double _usedLinkUtilization    (Algorithm&);
    double _coreLinkUtilization    (Algorithm&);
    double _aggLinkUtilization     (Algorithm&);
    double _edgeLinkUtilization    (Algorithm&);
    double _usedCoreLinkUtilization(Algorithm&);
    double _usedAggLinkUtilization (Algorithm&);
    double _usedEdgeLinkUtilization(Algorithm&);
    double _vnfResourcesUtilization(Algorithm&);
    void _writeHeaders  ();
    void _reportCost    (Event&, Algorithm&, Time&, EventProcessResult&);
    void _reportUtil    (Event&, Algorithm&, EventProcessResult&);
    void _reportTime    (Event&, Algorithm&, Time&, EventProcessResult&);
    void _reportAccp    (Event&, Algorithm&, EventProcessResult&);
    void _reportFlow    (Event&, Algorithm&);
    void _reportRejected(Event&);
    
public:
    Report(string, string, bool);
    
    void report(Event&, Algorithm&, Time&, Time&, EventProcessResult);
    
    ~Report();
};

