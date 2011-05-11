/*
Snapsort-Challenge -- An answer to the Snapsort coding challenge
Written by Chris Mennie (chris at chrismennie.ca or cmennie at rogers.com)
Copyright (C) 2011 Chris A. Mennie                              

License: Released under the GPL version 3 license. See the included LICENSE.
*/

#include "product.h"
#include "listing.h"
#include "datas.h"
#include "adhoc/adhoc.h"

#include <iostream>
#include <json/json.h>
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

namespace
{

//For JSON instance, grab a new listing
void importListing(Datas &datas, Json::Value &listingRoot)
{
    std::tr1::shared_ptr<Listing> newListing(new Listing);

    newListing->setTitleBase(listingRoot.get("title", "").asString());
    newListing->setManufacturerBase(listingRoot.get("manufacturer", "").asString());
    newListing->setCurrencyBase(listingRoot.get("currency", "").asString());
    newListing->setPriceBase(listingRoot.get("price", "").asString());

    newListing->setTitle(adhocStringNormalize(newListing->getTitleBase(), datas.stringTable));
    newListing->setManufacturer(adhocStringNormalize(newListing->getManufacturerBase(), datas.stringTable));
    newListing->setCurrency(adhocStringNormalize(newListing->getCurrencyBase(), datas.stringTable));
    newListing->setPrice(adhocStringNormalize(newListing->getPriceBase(), datas.stringTable));

    datas.addListing(newListing);
}//importListing

//For JSON instance, grab a new product
void importProduct(Datas &datas, Json::Value &productRoot)
{
    std::tr1::shared_ptr<Product> newProduct(new Product);

    newProduct->setProductNameBase(productRoot.get("product_name", "").asString());
    newProduct->setManufacturerBase(productRoot.get("manufacturer", "").asString());
    newProduct->setFamilyBase(productRoot.get("family", "").asString());
    newProduct->setModelBase(productRoot.get("model", "").asString());
    newProduct->setAnnouncedDateBase(productRoot.get("announced-date", "").asString());

    newProduct->setProductName(adhocStringNormalize(newProduct->getProductNameBase(), datas.stringTable));
    newProduct->setManufacturer(adhocStringNormalize(newProduct->getManufacturerBase(), datas.stringTable));
    newProduct->setFamily(adhocStringNormalize(newProduct->getFamilyBase(), datas.stringTable));
    newProduct->setModel(adhocStringNormalize(newProduct->getModelBase(), datas.stringTable));
    newProduct->setAnnouncedDate(adhocStringNormalize(newProduct->getAnnouncedDateBase(), datas.stringTable));

    datas.addProduct(newProduct);
}//importProduct

//Debug helper to verify imported data was correct
void dumpData(Datas &datas)
{
    BOOST_FOREACH (std::tr1::shared_ptr<Listing> listing, datas.getListingPair()) {
        listing->dump();
    }//foreach

    std::cout << std::endl << std::endl;

    BOOST_FOREACH (std::tr1::shared_ptr<Product> product, datas.getProductPair()) {
        product->dump();
    }//foreach    
}//dumpData

//When outputting JSON we need to make sure quotations are escaped. Hopefully that's the only case to consider.
std::string escapeQuotes(const std::string &str)
{
    std::string retStr;

    retStr.reserve(str.size() * 2);
    for (unsigned int pos = 0; pos < str.size(); ++pos) {
        char curChar = str[pos];

        if (curChar == '\"') {
            retStr.push_back('\\');
        }//if

        retStr.push_back(curChar);
    }//for

    return retStr;
}//escapeQuotes

//Create the result file
void outputResults(Datas &datas)
{
    std::ofstream outFile("results.json");

    typedef std::pair<std::tr1::shared_ptr<Product>, std::vector<std::pair<std::tr1::shared_ptr<Listing>, float> > > ResultMapPair;
    typedef std::pair<std::tr1::shared_ptr<Listing>, float> ResultListingPair;

    float acceptanceThreshold = 0.695f;

    //For each product, dump out an entry listing every matching listing which passes the acceptance threshold
    BOOST_FOREACH (std::tr1::shared_ptr<ResultHolder> resultHolder, datas.getResultHolderPair()) {
        std::string &productName = resultHolder->getProduct()->getProductNameBase();
        outFile << "{ \"product_name\" : \"" << escapeQuotes(productName) << "\", " ;
        outFile << "\"listings\": [";

        bool firstListing = true;
        unsigned int numListings = resultHolder->getListings().size();
        for (unsigned int pos = 0; pos < numListings; ++pos) {
            if (resultHolder->getWeights()[pos] < acceptanceThreshold) {
                continue;
            }//if

            std::tr1::shared_ptr<Listing> curListing = resultHolder->getListings()[pos];

            if (false == firstListing) {
                outFile << ", ";
            } else {
                firstListing = false;
            }//if

            outFile << "{ ";
            outFile << "\"title\" : \"" << escapeQuotes(curListing->getTitleBase()) << "\", ";
            outFile << "\"manufacturer\" : \"" << escapeQuotes(curListing->getManufacturerBase()) << "\", ";
            outFile << "\"currency\" : \"" << escapeQuotes(curListing->getCurrencyBase()) << "\", ";
            outFile << "\"price\" : \"" << escapeQuotes(curListing->getPriceBase()) << "\" ";
            outFile << "}";
        }//foreach

        outFile << "]}" << std::endl;
    }//foreach

    outFile.close();
}//outputResults

//Create a nicely formatted result file for debugging purposes
void outputResultsFormatted(Datas &datas)
{
    std::ofstream outFile("results.json");

    typedef std::pair<std::tr1::shared_ptr<Product>, std::vector<std::pair<std::tr1::shared_ptr<Listing>, float> > > ResultMapPair;
    typedef std::pair<std::tr1::shared_ptr<Listing>, float> ResultListingPair;

    float acceptanceThreshold = 0.695f;

    BOOST_FOREACH (std::tr1::shared_ptr<ResultHolder> resultHolder, datas.getResultHolderPair()) {
        std::string &productName = resultHolder->getProduct()->getProductNameBase();
        outFile << "{ \"product_name\" : \"" << escapeQuotes(productName) << "\", " << std::endl;
        outFile << "   \"listings\": [" << std::endl;

        bool firstListing = true;
        unsigned int numListings = resultHolder->getListings().size();
        for (unsigned int pos = 0; pos < numListings; ++pos) {
            if (resultHolder->getWeights()[pos] < acceptanceThreshold) {
                //results may not be sorted, so we continue instead of break
                continue;
            }//if

            std::tr1::shared_ptr<Listing> curListing = resultHolder->getListings()[pos];

            if (false == firstListing) {
                outFile << "," << std::endl;
            } else {
                firstListing = false;
            }//if

            outFile << "     {" << std::endl;
            outFile << "          \"title\" : \"" << escapeQuotes(curListing->getTitleBase()) << "\", " << std::endl;
            outFile << "          \"manufacturer\" : \"" << escapeQuotes(curListing->getManufacturerBase()) << "\", " << std::endl;
            outFile << "          \"currency\" : \"" <<  escapeQuotes(curListing->getCurrencyBase()) << "\", " << std::endl;
            outFile << "          \"price\" : \"" << escapeQuotes(curListing->getPriceBase()) << "\"" << std::endl;
            outFile << "     }" << std::endl;
        }//foreach

        outFile << "   ]}" << std::endl;        
    }//foreach

    outFile.close();
}//outputResultsFormatted

//Debug helper to verify I correctly wrote out the results
void verifyWrittenJSON()
{
    std::string inLine;
    std::ifstream resultsFile("results.json");
    while (std::getline(resultsFile, inLine)) {
        if (inLine.length() == 0) {
            continue;
        }//if

        Json::Value productRoot;
        Json::Reader productReader;
        bool parsingSuccessful = productReader.parse(inLine, productRoot);
        if (false == parsingSuccessful) {
            // report to the user the failure and their locations in the document.
            std::cout << "Failed to parse results configuration" << std::endl << productReader.getFormattedErrorMessages();
            std::cout << "line was: '" << inLine << "'" << std::endl;
            return;
        }//if
    }//while
}//verifyWrittenJSON

}//anonymous namespace

int main(int argc, const char* argv[])
{
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " <listings.txt> <products.txt> <numThreads>" << std::endl;
        return -1;
    }//if

    unsigned int numThreads = boost::lexical_cast<unsigned int>(argv[3]);

    std::ifstream listingFile(argv[1]);
    std::ifstream productFile(argv[2]);

    Datas datas;
    
    //Soak up the listing data
    std::string inLine;
    while (std::getline(listingFile, inLine)) {
        if (inLine.length() == 0) {
            continue;
        }//if

        Json::Value listingRoot;
        Json::Reader listingReader;
        bool parsingSuccessful = listingReader.parse(inLine, listingRoot);
        if (false == parsingSuccessful) {
            // report to the user the failure and their locations in the document.
            std::cout  << "Failed to parse listing configuration" << std::endl << listingReader.getFormattedErrorMessages();
            std::cout << "line was: '" << inLine << "'" << std::endl;
            return -1;
        }//if

        importListing(datas, listingRoot);
    }//while

    //Soak up the product data
    while (std::getline(productFile, inLine)) {
        if (inLine.length() == 0) {
            continue;
        }//if

        Json::Value productRoot;
        Json::Reader productReader;
        bool parsingSuccessful = productReader.parse(inLine, productRoot);
        if (false == parsingSuccessful) {
            // report to the user the failure and their locations in the document.
            std::cout  << "Failed to parse product configuration" << std::endl << productReader.getFormattedErrorMessages();
            std::cout << "line was: '" << inLine << "'" << std::endl;
            return -1;
        }//if

        importProduct(datas, productRoot);
    }//while

    //dumpData(datas); -- for debugging

    //Start the magic happening
    doAdhocMatching(datas, numThreads);
    outputResults(datas);

    //outputResultsFormatted(datas); -- for debugging
    //verifyWrittenJSON(); -- for debugging

    std::cout << "Finished." << std::endl;

    return 0;
}//main

