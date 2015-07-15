//
//  resource.h
//  VNFPlacement
//
//  Created by Milad Ghaznavi on 5/1/15.
//  Copyright (c) 2015 Milad Ghaznavi. All rights reserved.
//

#pragma once

class ResourceBase {
public:
    #define DEFAULT_INIT_ALLOC   0
    #define DEFAULT_CAPACITY     0
    #define DEFAULT_ALLOCATION   1
    #define DEFAULT_RELEASE      1
    
private:
    int  _capacity;
    int  _allocated;
    
protected:
    void capacity_(int);
    void init_    (int, int);
    
public:
    ResourceBase         (int = DEFAULT_INIT_ALLOC);
    int  capacity        ();
    int  residualCapacity();
    int  allocated       ();
    void allocate        (int = DEFAULT_ALLOCATION);
    void release         (int = DEFAULT_RELEASE);
    void releaseAll      ();
    
    operator int ();
};