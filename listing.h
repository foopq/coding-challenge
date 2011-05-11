/*
Snapsort-Challenge -- An answer to the Snapsort coding challenge
Written by Chris Mennie (chris at chrismennie.ca or cmennie at rogers.com)
Copyright (C) 2011 Chris A. Mennie                              

License: Released under the GPL version 3 license. See the included LICENSE.
*/

#ifndef __LISTING_H
#define __LISTING_H

#include <string>
#include <vector>
#include <tr1/memory>
#include <boost/thread.hpp>

struct Product;

//Represents a listing. We also keep track of the best matched product for this listing.
class Listing
{
    std::string titleBase;
    std::string manufacturerBase;
    std::string currencyBase;
    std::string priceBase;

    std::vector<unsigned int> title;
    std::vector<unsigned int> manufacturer;
    std::vector<unsigned int> currency;
    std::vector<unsigned int> price;

    std::tr1::shared_ptr<Product> bestMatchedProduct;
    float bestMatchedWeight;
    boost::mutex listingLock;

public:    
    Listing()
    {
        bestMatchedWeight = -99999.0f;
    }//constuctor

    boost::mutex &getListingLock() { return listingLock; }

    std::string &getTitleBase() { return titleBase; }
    std::string &getManufacturerBase() { return manufacturerBase; }
    std::string &getCurrencyBase() { return currencyBase; }
    std::string &getPriceBase() { return priceBase; }

    std::vector<unsigned int> &getTitle() { return title; }
    std::vector<unsigned int> &getManufacturer() { return manufacturer; }
    std::vector<unsigned int> &getCurrency() { return currency; }
    std::vector<unsigned int> &getPrice() { return price; }

    std::tr1::shared_ptr<Product> getBestMatchedProduct() { return bestMatchedProduct; }
    float getBestMatchedWeight() { return bestMatchedWeight; }

    void setBestMatchedProduct(std::tr1::shared_ptr<Product> product) { bestMatchedProduct = product; }
    void setBestMatchedWeight(float weight) { bestMatchedWeight = weight; }

    void setTitleBase(const std::string &str) { titleBase = str; }
    void setManufacturerBase(const std::string &str) { manufacturerBase = str; }
    void setCurrencyBase(const std::string &str) { currencyBase = str; }
    void setPriceBase(const std::string &str) { priceBase = str; }

    void setTitle(std::vector<unsigned int> vec) { title = vec; }
    void setManufacturer(std::vector<unsigned int> vec) { manufacturer = vec; }
    void setCurrency(std::vector<unsigned int> vec) { currency = vec; }
    void setPrice(std::vector<unsigned int> vec) { price = vec; }

    void dump();
};//Listing

#endif


