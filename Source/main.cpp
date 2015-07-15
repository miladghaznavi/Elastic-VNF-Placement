//
//  main.cpp
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 4/18/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include <iostream>
#include <string>
#include <stdexcept>
#include "network.h"
#include "simulator.h"
#include "slflsim.h"
#include "firstfitalgsim.h"
#include "randalgsim.h"
#include "lowerboundsim.h"
#include "cxxopts.hpp"
#include "mapping.h"

using namespace std;
using namespace cxxopts;

#define PROGRAM_NAME  "VNFPlacement"
#define CONF_OPT      "c"
#define L_CONF_OPT    "config"
#define ALG_OPT       "a"
#define L_ALG_OPT     "algorithm"
#define SLFL_ALG      "slfl"
#define FIRST_FIT_ALG "firstfit"
#define RAND_ALG      "random"
#define LOWERBND_ALG  "lowerbound"
#define VERBOSE_OPT   "v"
#define L_VERBOSE_OPT "verbose"

#define DEF_VERBOSE_MODE false
#define LOG_PATH         "log.txt"

void parseArgs(int, char*[], Options&);
void whichSimulator(string, string, Simulator*&);

int main(int argc, char * argv[]) {
    streambuf *coutbuf = std::cout.rdbuf();

    Simulator* simulator = NULL;
    Options options(PROGRAM_NAME);
    string configPath, algorithm;
    bool   verboseMode = DEF_VERBOSE_MODE;
    try {
        parseArgs(argc, argv, options);
        if (options.count(CONF_OPT) != 1 || options.count(ALG_OPT) != 1)
            throw exception();
        configPath  = options[CONF_OPT].as<string>();
        algorithm   = options[ALG_OPT].as<string>();
        verboseMode = options.count(VERBOSE_OPT);
    }//try
    catch(...) {
        cerr << options.help() << endl;
        exit(-1);
    }//catch
    
    try {
        // Verbose mode
        if (!verboseMode) {
            ofstream log(LOG_PATH);
            cout.rdbuf(log.rdbuf());
        }//if
        
        // which simulator
        whichSimulator(algorithm, configPath, simulator);
        
        // Init
        cout << "Initializing simulator of '" << algorithm << "'...";
        simulator->init();

        // Start Simulator
        cout << "Start " << algorithm << "..." << endl;
        simulator->start();
        cout << endl << "Simulation has successfuly finished!" << endl;
    }//try
    catch(logic_error logicError) {
        cerr << logicError.what() << endl;
        exit(-1);
    }//catch
    catch(invalid_argument argError) {
        cerr << argError.what() << endl;
        exit(-1);
    }//catch
    catch(...) {
        cerr << "Error!" << endl;
        exit(-1);
    }//catch
    
    try {
        cout.rdbuf(coutbuf);
    } catch(...) { }//catch
    
    return 0;
}


void parseArgs(int argc, char* argv[], Options& options) {
    options.add_options()
        (CONF_OPT "," L_CONF_OPT,
            "Configuration File",
            value<string>())
        (ALG_OPT  "," L_ALG_OPT ,
            "Algortithm <" SLFL_ALG ", " FIRST_FIT_ALG ", " RAND_ALG ", " LOWERBND_ALG ">",
            value<string>())
        (VERBOSE_OPT  "," L_VERBOSE_OPT,
            "Verbose Mode");
    options.parse(argc, argv);
}

void whichSimulator(string algorithm, string configPath, Simulator*& simulator) {
    if (algorithm == SLFL_ALG) {
        simulator = new SLFLSim(configPath);
    }//if
    else if (algorithm == FIRST_FIT_ALG) {
        simulator = new FirstFitAlgSim(configPath);
    }//else if
    else if (algorithm == RAND_ALG) {
        simulator = new RandAlgorithmSim(configPath);
    }//else if
    else if (algorithm == LOWERBND_ALG) {
        simulator = new LowerBoundAlgSim(configPath);
    }//else if
    else {
        stringstream ss;
        ss << "No algorithm " << algorithm << " exits!";
        throw invalid_argument(ss.str());
    }//else
}
