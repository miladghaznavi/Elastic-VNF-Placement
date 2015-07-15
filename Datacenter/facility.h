//
//  facility.h
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 4/18/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#pragma once
#include "basics.h"
#include "resource.h"

class Facility : public BaseClass, public ResourceBase {
public:
    #define DEFAULT_LOCATION     INVALID_ID
    
private:
    Identifier _location;
    
protected:
    static int facilitiesCount_;
    
    void out_ (std::ostream&);
    void in_  (std::istream&);
    void init_(Identifier, Identifier, int, int);

public:
    Facility(Identifier, int);
    Facility(int = DEFAULT_CAPACITY);
    
    Identifier location();
    void       location(Identifier);
};
