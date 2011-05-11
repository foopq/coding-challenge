/*
Snapsort-Challenge -- An answer to the Snapsort coding challenge
Written by Chris Mennie (chris at chrismennie.ca or cmennie at rogers.com)
Copyright (C) 2011 Chris A. Mennie                              

License: Released under the GPL version 3 license. See the included LICENSE.
*/

#include "adhoc.h"
#include "../stringTable.h"
#include <sstream>
#include <boost/foreach.hpp>
#include <boost/array.hpp>
#include <limits>
#include <ctype.h>

namespace
{

//If we had a large number we could turn this into a hash table
//This represents words that we're confident have no meaning and would be
//better removed
const unsigned int numNoiseEntries = 9;
boost::array<std::string, numNoiseEntries> NoiseEntries = {{
    "gmbh", "inc", "ltd", "uk", "corporation", "international", "llc", "co", "plc"
}};//NoiseEntries

bool isPunctuation(char c)
{
    if (ispunct(c) != 0) {
        return true;
    } else {
        switch (c) {
            case '(':
            case ')':
                return true;

            default:
                ;
                //Nothing
        }//switch

        return false;
    }//if
}//isPunctuation

//Given a string, remove leading and trailing punctuation and filter out 
//common "useless" words
std::string transmuteString(const std::string &str)
{
    if (str.empty() == true) {
        return "";
    }//if

    //remove leading and trailing punctuation
    unsigned int firstNonPunctuationPos = std::numeric_limits<unsigned int>::max();
    unsigned int lastNonPunctuationPos = 0;
    for (unsigned int pos = 0; pos < str.length(); ++pos) {
        if ((std::numeric_limits<unsigned int>::max() == firstNonPunctuationPos) && (isPunctuation(str[pos]) == false)) {
            firstNonPunctuationPos = pos;
        }//if

        //Could duplicate the loop and march backwards to save ourselves a little bit of work
        if (isPunctuation(str[pos]) == false) {
            lastNonPunctuationPos = pos;
        }//if
    }//foreach

    if (std::numeric_limits<unsigned int>::max() == firstNonPunctuationPos) {
        return "";
    }//if

    if (firstNonPunctuationPos == lastNonPunctuationPos) {
        //Shouldn't get in here
        return "";
    }//if

    std::string curStr = str.substr(firstNonPunctuationPos, lastNonPunctuationPos - firstNonPunctuationPos + 1);

    //filter out common noise
    bool isNoise = false;
    BOOST_FOREACH (std::string &noiseStr, NoiseEntries) {
        if (curStr == noiseStr) {
            isNoise = true;
            break;
        }//if
    }//foreach
    
    if (true == isNoise) {
        return "";
    }//if

    //The normalized string
    return curStr;
}//transmuteString

}//anonymous namespace

//Normalize a string into a set of words represented as a vector of ints
std::vector<unsigned int> adhocStringNormalize(const std::string &str, StringTable &stringTable)
{
    std::vector<unsigned int> retStr;

    if (str.empty() == true) {
        return retStr;
    }//if

    std::vector<std::string> stringBits;

    //Gather up vector of words from original string
    std::istringstream tmpSS(str);
    do {
        std::string subStr;
        tmpSS >> subStr;

        //Everything lower case
        std::transform(subStr.begin(), subStr.end(), subStr.begin(), ::tolower);

        //Remove any dashes in middle of string
        size_t dashPos;
        do {
            dashPos = subStr.find('-');

            if (dashPos != std::string::npos) {
                subStr.erase(dashPos, 1);
            }//if
        } while (dashPos != std::string::npos);

        stringBits.push_back(subStr);
    } while(tmpSS);

    //For each word, normalize it and then place it into our string table
    BOOST_FOREACH (std::string &subStr, stringBits) {
        std::string transmutedString = transmuteString(subStr);
        if (transmutedString.empty() == false) {
            retStr.push_back(stringTable.getStringVal(subStr));
        }//if
    }//foreach

    return retStr;
}//adhocStringNormalize

