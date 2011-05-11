/*
Snapsort-Challenge -- An answer to the Snapsort coding challenge
Written by Chris Mennie (chris at chrismennie.ca or cmennie at rogers.com)
Copyright (C) 2011 Chris A. Mennie                              

License: Released under the GPL version 3 license. See the included LICENSE.
*/


#include "listing.h"
#include <iostream>

void Listing::dump()
{
        std::cout << "title: " << titleBase << std::endl;
        std::cout << "manufacturer: " << manufacturerBase << std::endl;
        std::cout << "currency: " << currencyBase << std::endl;
        std::cout << "price: " << priceBase << std::endl;
        std::cout << std::endl;
}//dump

