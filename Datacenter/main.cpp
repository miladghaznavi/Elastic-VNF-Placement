//
//  main.cpp
//  DataCenterTopo
//
//  Created by Milad Ghaznavi on 4/17/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include <iostream>
#include <string>
#include <stdexcept>
#include "cxxopts.hpp"
#include "fattree.h"
#include "graphio.h"
#include "substrategraph.h"

using namespace std;
using namespace cxxopts;

#define PROGRAM_NAME    "dc"
#define K_OPT           "k"
#define L_K_OPT         "kparam"
#define LINK_CAP_OPT    "l"
#define L_LINK_CAP_OPT  "link"
#define HOST_CAP_OPT    "h"
#define L_HOST_CAP_OPT  "host"
#define OUTPUT_OPT      "o"
#define L_OUTPUT_OPT    "output"

//Data center
#define DEF_DC_FILE_NAME "dc.txt"
#define DEF_LINK_WEIGHT  1

void parseArgs(int, char*[], Options&);

void checkArgs(int, int, int, string);

int main(int argc, char * argv[]) {
    int    k, l, h;
    string o;
    Options options(PROGRAM_NAME);
    try {
        parseArgs(argc, argv, options);
        if (options.count(K_OPT) != 1 || options.count(LINK_CAP_OPT) != 1 || options.count(HOST_CAP_OPT) != 1)
            throw exception();
        
        k = options[K_OPT       ].as<int>   ();
        l = options[LINK_CAP_OPT].as<int>   ();
        h = options[HOST_CAP_OPT].as<int>   ();
        o = (options.count(OUTPUT_OPT) > 0) ? options[OUTPUT_OPT].as<string>() : DEF_DC_FILE_NAME;

        KAryFatTree topology(k);
        cout << "K        : " << k                  << endl;
        cout << "Link Cap.: " << l                  << endl;
        cout << "Host Cap.: " << h                  << endl;
        cout << "# Nodes  : " << topology.nodeNum() << endl;
        cout << "# Hosts  : " << topology.hostNum() << endl;
        cout << "# Edges  : " << topology.edgeNum() << endl;
        
        SubstrateGraph network(topology);
        for (auto n = topology.hostIt(); n != INVALID; ++n) {
            network.capacity(n, h);
        }//for
        
        for (EdgeIt e(network); e != INVALID; ++e) {
            network.capacity(e, l);
            network.weight  (e, DEF_LINK_WEIGHT);
        }//for
        
        GraphIO::writeGraph(network, o);
        cout << "Saved in : '" << o << "'!" << endl;
    }//try
    catch(invalid_argument inExc) {
        cerr << inExc.what() << endl;
        exit(-1);
    }//catch
    catch(...) {
        cerr << options.help() << endl;
        exit(-1);
    }//catch

    return 0;
}

void parseArgs(int argc, char* argv[], Options& options) {
    options.add_options()
        (K_OPT        "," L_K_OPT       , "K parameter"                      , value<int>())
        (LINK_CAP_OPT "," L_LINK_CAP_OPT, "Link Capacity (in MB)"            , value<int>())
        (HOST_CAP_OPT "," L_HOST_CAP_OPT, "Host Capacity (in Number of CPUs)", value<int>())
        (OUTPUT_OPT   "," L_OUTPUT_OPT  , "Output File (Default is dc.txt)"  , value<string>());
    options.parse(argc, argv);
}

void checkArgs(int k, int l, int h, string o) {
    if (k < 0) {
        stringstream ss;
        ss << "Parameter k cannot be negative";
        throw invalid_argument(ss.str());
    }//if
}

