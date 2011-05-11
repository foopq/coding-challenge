/*
Snapsort-Challenge -- An answer to the Snapsort coding challenge
Written by Chris Mennie (chris at chrismennie.ca or cmennie at rogers.com)
Copyright (C) 2011 Chris A. Mennie                              

License: Released under the GPL version 3 license. See the included LICENSE.
*/


#include "stringTable.h"

StringTable::StringTable()
{
    table[0] = "";
}//constructor

//Returns the table entry for a string, adding it to the table if it wasn't already there
unsigned int StringTable::getStringVal(const std::string &str)
{
    if (tableRev.find(str) != tableRev.end()) {
        return tableRev[str];
    }//if

    unsigned int newId = table.size() + 1;
    tableRev[str] = newId;
    table[newId] = str;

    return newId;
}//getStringVal

//Returns the string for a given key in the table
std::string &StringTable::getString(unsigned int key)
{
    std::unordered_map<unsigned int, std::string>::iterator tableIter = table.find(key);

    if (tableIter != table.end()) {
        return tableIter->second;
    } else {
        return table[0]; //""
    }//if
}//getString



