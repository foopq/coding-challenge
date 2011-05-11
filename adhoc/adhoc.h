/*
Snapsort-Challenge -- An answer to the Snapsort coding challenge
Written by Chris Mennie (chris at chrismennie.ca or cmennie at rogers.com)
Copyright (C) 2011 Chris A. Mennie                              

License: Released under the GPL version 3 license. See the included LICENSE.
*/

#ifndef __ADHOC_H
#define __ADHOC_H

#include <vector>
#include <string>

#include "../stringTable.h"
#include "../datas.h"

//Normalize a string
std::vector<unsigned int> adhocStringNormalize(const std::string &str, StringTable &stringTable);

//Determine the product->listings matchings. Spawn off N threads and go from there.
void doAdhocMatching(Datas &datas, unsigned int numThreads);

#endif
