//
//  basics.h
//  ChainEmbedding
//
//  Created by Milad Ghaznavi on 3/19/15.
//  Copyright (c) 2015 Milad Ghaznavi, Nashid Shahriar. All rights reserved.
//

#pragma once

#include <fstream>

#define INVALID_ID  -1
typedef int Identifier;

class BaseClass {
private:
    Identifier _identifier;

public:
    Identifier identifier();
    void identifier(Identifier);
    
protected:
    virtual void in_ (std::istream&);
    virtual void out_(std::ostream&);
    
    static std::ofstream& createFile_(std::string);
    static std::ifstream& openFile_  (std::string);
    
public:
    virtual void readFromStream    (std::istream&);
    virtual void writeToStream     (std::ostream&);
    
    friend std::ostream& operator<<(std::ostream&, BaseClass&);
    friend std::istream& operator>>(std::istream&, BaseClass&);
};
