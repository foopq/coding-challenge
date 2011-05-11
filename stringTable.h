/*
Snapsort-Challenge -- An answer to the Snapsort coding challenge
Written by Chris Mennie (chris at chrismennie.ca or cmennie at rogers.com)
Copyright (C) 2011 Chris A. Mennie                              

License: Released under the GPL version 3 license. See the included LICENSE.
*/

#ifndef __STRINGTABLE_H
#define __STRINGTABLE_H

#include <unordered_map>
#include <string>

//A simple string table
class StringTable
{
    std::unordered_map<unsigned int, std::string> table;
    std::unordered_map<std::string, unsigned int> tableRev;

public:
    StringTable();

    unsigned int getStringVal(const std::string &str);
    std::string &getString(unsigned int key);
};//StringTable

#endif


