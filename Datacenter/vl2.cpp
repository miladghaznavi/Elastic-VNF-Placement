//
//  vl2.cpp
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 5/2/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#include "vl2.h"

void VirtualLayer2::_build() {
    //generate all nodes
    int count =  intermedSwitchNum() +
        aggSwitchNum() +
        ToRSwitchNum() +
        hostNum();
    
    for (int i = 0; i < count; i++)
        this->addNode();
    
    //Connect intermediate switches to aggregate switches
    for (int inSwitch = 0; inSwitch < intermedSwitchNum(); inSwitch++)
        for (int aggSwitch = firstAggSwitch_(); aggSwitch < firstToRSwitch_(); aggSwitch++)
            this->addEdge(nodeFromId(inSwitch), nodeFromId(aggSwitch));
    
    //Connect aggregation switches to ToR swiches
    for (int ToR = firstToRSwitch_(); ToR < firstHost_(); ToR++) {
        for (int p = 0; p < ddt(); p++) {
            int host = firstHost_() + p + (ToR - firstToRSwitch_()) / 2 * ddt();
            this->addEdge(nodeFromId(ToR), nodeFromId(host));
        }//for
    }//for
    
    // Connect ToR switchees to hosts
    int hcount = firstHost_();
    for (int i = firstToRSwitch_(); i < firstHost_(); i++)
        for (int j = 0; j < ddt(); j++)
            this->addEdge(nodeFromId(i), nodeFromId(hcount++));
}

VirtualLayer2::VirtualLayer2 (int di, int da, int ddt) {
    if (di % 2 != 0 || di < 0) {
        throw std::invalid_argument("Invalid di (degree of intermediate switch)!");
    }//if
    if (da % 2 != 0 || da < 0) {
        throw std::invalid_argument("Invalid da (degree of aggregation switch)!");
    }//if
    
    if (ddt < 0) {
        throw std::invalid_argument("Invalid ddt!");
    }
    
    this->_di = di;
    this->_da = da;
    this->_ddt = ddt;
    
    _build();
}

int VirtualLayer2::di() {
    return _di;
}

int VirtualLayer2::da() {
    return _da;
}

int VirtualLayer2::ddt() {
    return _ddt;
}

int VirtualLayer2::intermedSwitchNum() {
    return da() / 2;
}

int VirtualLayer2::aggSwitchNum() {
    return di();
}

int VirtualLayer2::ToRSwitchNum() {
    return da() * di() / 4;
}

int VirtualLayer2::hostNum () {
    return ToRSwitchNum() * ddt();
}

Identifier VirtualLayer2::firstIntermedSwitch_() {
    return 0;
}

Identifier VirtualLayer2::firstAggSwitch_() {
    return firstIntermedSwitch_() + intermedSwitchNum();
}

Identifier VirtualLayer2::firstToRSwitch_() {
    return firstAggSwitch_() + aggSwitchNum();
}

Identifier VirtualLayer2::firstHost_() {
    return firstToRSwitch_() + ToRSwitchNum();
}

Identifier VirtualLayer2::lastIntermedSwitch_() {
    return firstIntermedSwitch_() + intermedSwitchNum() - 1;
}

Identifier VirtualLayer2::lastAggSwitch_() {
    return firstAggSwitch_() + aggSwitchNum() - 1;
}

Identifier VirtualLayer2::lastToRSwitch_() {
    return firstToRSwitch_() + ToRSwitchNum() - 1;
}

Identifier VirtualLayer2::lastHost_() {
    return firstHost_() + hostNum() - 1;
}

VirtualLayer2::NodeRangeIt VirtualLayer2::intermedSwitchIt() {
    return NodeRangeIt(*this,
                       this->nodeFromId(intermedSwitchNum() - 1),
                       Node(Invalid()));
}

VirtualLayer2::NodeRangeIt VirtualLayer2::aggSwitchIt() {
    return NodeRangeIt(*this,
                       this->nodeFromId(intermedSwitchNum() + aggSwitchNum() - 1),
                       this->nodeFromId(intermedSwitchNum() - 1));
}

VirtualLayer2::NodeRangeIt VirtualLayer2::ToRSwitchIt() {
    return NodeRangeIt(*this,
                       this->nodeFromId(intermedSwitchNum() + aggSwitchNum() + ToRSwitchNum() - 1),
                       this->nodeFromId(intermedSwitchNum() + aggSwitchNum() - 1));
}

VirtualLayer2::NodeRangeIt VirtualLayer2::hostIt() {
    return NodeRangeIt(*this,
                       this->nodeFromId(intermedSwitchNum() + aggSwitchNum() + ToRSwitchNum() + hostNum() - 1),
                       this->nodeFromId(intermedSwitchNum() + aggSwitchNum() + ToRSwitchNum() - 1));
}

