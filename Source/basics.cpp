//
//  basics.cpp
//  ChainEmbedding
//
//  Created by Milad Ghaznavi on 3/19/15.
//  Copyright (c) 2015 Milad Ghaznavi, Nashid Shahriar. All rights reserved.
//

#include "basics.h"
#include <sstream>
#include <fstream>
#include <stdexcept>
using namespace std;

void BaseClass::in_ (istream& is) { }

void BaseClass::out_(ostream& os) { }

Identifier BaseClass::identifier() {
    return _identifier;
}

void BaseClass::identifier(Identifier id) {
    this->_identifier = id;
}

ofstream& BaseClass::createFile_(string path) {
    ofstream* file = new ofstream(path.c_str());
    if (!(*file)) {
        stringstream ss;
        ss << "Unable to create file '" << path << "'!";
        throw runtime_error(ss.str());
    }//if
    
    return *file;
}

ifstream& BaseClass::openFile_(string path) {
    ifstream* file = new ifstream(path.c_str());
    if (!(*file)) {
        stringstream ss;
        ss << "Unable to open file '" << path << "'!";
        throw runtime_error(ss.str());
    }//if
    
    return *file;
}

void BaseClass::readFromStream(istream& is) {
    in_(is);
}

void BaseClass::writeToStream(ostream& os) {
    out_(os);
}

ostream& operator<<(ostream& os, BaseClass& bc) {
    bc.out_(os);
    return os;
}

istream& operator>>(istream& is, BaseClass& bc) {
    bc.in_(is);
    return is;
}
