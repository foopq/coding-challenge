/*
Snapsort-Challenge -- An answer to the Snapsort coding challenge
Written by Chris Mennie (chris at chrismennie.ca or cmennie at rogers.com)
Copyright (C) 2011 Chris A. Mennie                              

License: Released under the GPL version 3 license. See the included LICENSE.
*/


#ifndef __PRODUCT_H
#define __PRODUCT_H

#include <string>
#include <vector>
#include <tr1/memory>

struct Listing;

//Represents a product
class Product
{
    std::string productNameBase;
    std::string manufacturerBase;
    std::string familyBase;
    std::string modelBase;
    std::string announcedDateBase;

    std::vector<unsigned int> productName;
    std::vector<unsigned int> manufacturer;
    std::vector<unsigned int> family;
    std::vector<unsigned int> model;
    std::vector<unsigned int> announcedDate;

    std::vector<std::tr1::shared_ptr<Listing> > matchedListings;

public:    
    std::string &getProductNameBase() { return productNameBase; }
    std::string &getManufacturerBase() { return manufacturerBase; }
    std::string &getFamilyBase() { return familyBase; }
    std::string &getModelBase() { return modelBase; }
    std::string &getAnnouncedDateBase() { return announcedDateBase; }

    std::vector<unsigned int> &getProductName() { return productName; }
    std::vector<unsigned int> &getManufacturer() { return manufacturer; }
    std::vector<unsigned int> &getFamily() { return family; }
    std::vector<unsigned int> &getModel() { return model; }
    std::vector<unsigned int> &getAnnouncedDate() { return announcedDate; }

    void setProductNameBase(const std::string &str) { productNameBase = str; }
    void setManufacturerBase(const std::string &str) { manufacturerBase = str; }
    void setFamilyBase(const std::string &str) { familyBase = str; }
    void setModelBase(const std::string &str) { modelBase = str; }
    void setAnnouncedDateBase(const std::string &str) { announcedDateBase = str; }

    void setProductName(std::vector<unsigned int> vec) { productName = vec; }
    void setManufacturer(std::vector<unsigned int> vec) { manufacturer = vec; }
    void setFamily(std::vector<unsigned int> vec) { family = vec; }
    void setModel(std::vector<unsigned int> vec) { model = vec; }
    void setAnnouncedDate(std::vector<unsigned int> vec) { announcedDate = vec; }

    std::pair<std::vector<std::tr1::shared_ptr<Listing> >::iterator, std::vector<std::tr1::shared_ptr<Listing> >::iterator> getMatchedListingPair()
    {
        return std::make_pair(matchedListings.begin(), matchedListings.end());
    }//getMatchedListingPair

    void addMatchedListing(std::tr1::shared_ptr<Listing> listing) { matchedListings.push_back(listing); }

    void dump();
};//Product

#endif


