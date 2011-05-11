/*
Snapsort-Challenge -- An answer to the Snapsort coding challenge
Written by Chris Mennie (chris at chrismennie.ca or cmennie at rogers.com)
Copyright (C) 2011 Chris A. Mennie                              

License: Released under the GPL version 3 license. See the included LICENSE.
*/

#ifndef __DATAS_H
#define __DATAS_H

#include <vector>
#include <tr1/memory>
#include <map>
#include "stringTable.h"

struct Listing;
struct Product;

//Represents the list of listings for a given product (and their associated weight)
class ResultHolder
{
    std::tr1::shared_ptr<Product> product;
    std::vector<std::tr1::shared_ptr<Listing> > listings;
    std::vector<float> weights;

public:    
    std::tr1::shared_ptr<Product> getProduct() { return product; }
    std::vector<std::tr1::shared_ptr<Listing> > &getListings() { return listings; }
    std::vector<float> &getWeights() { return weights; }

    void setProduct(std::tr1::shared_ptr<Product> product_) { product = product_; }

    void addListing(std::tr1::shared_ptr<Listing> listing) { listings.push_back(listing); }
    void addWeight(float weight) { weights.push_back(weight); }    

    void reserveListings(unsigned int size) { listings.reserve(size); }
    void reserveWeights(unsigned int size) { weights.reserve(size); }
};//ResultHolder

//Holds all the listings, products, and results
class Datas
{
    std::vector<std::tr1::shared_ptr<Listing> > listings;
    std::vector<std::tr1::shared_ptr<Product> > products;

    std::vector<std::tr1::shared_ptr<ResultHolder> > results;

public:    
    StringTable stringTable; //we don't need to have a lock around this

    void addListing(std::tr1::shared_ptr<Listing> listing) { listings.push_back(listing); }
    void addProduct(std::tr1::shared_ptr<Product> product) { products.push_back(product); }
    void addResult(std::tr1::shared_ptr<ResultHolder> result) { results.push_back(result); }

    std::pair<std::vector<std::tr1::shared_ptr<Listing> >::iterator, std::vector<std::tr1::shared_ptr<Listing> >::iterator> getListingPair() 
    { 
        return std::make_pair(listings.begin(), listings.end()); 
    }//getListingPair

    std::pair<std::vector<std::tr1::shared_ptr<Product> >::iterator, std::vector<std::tr1::shared_ptr<Product> >::iterator> getProductPair() 
    { 
        return std::make_pair(products.begin(), products.end()); 
    }//getProductPair

    std::pair<std::vector<std::tr1::shared_ptr<ResultHolder> >::iterator, std::vector<std::tr1::shared_ptr<ResultHolder> >::iterator> getResultHolderPair() 
    { 
        return std::make_pair(results.begin(), results.end()); 
    }//getListingPair
};//Datas

#endif


