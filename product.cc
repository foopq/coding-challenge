/*
Snapsort-Challenge -- An answer to the Snapsort coding challenge
Written by Chris Mennie (chris at chrismennie.ca or cmennie at rogers.com)
Copyright (C) 2011 Chris A. Mennie                              

License: Released under the GPL version 3 license. See the included LICENSE.
*/


#include "product.h"
#include <iostream>

void Product::dump()
{
        std::cout << "productName: " << productNameBase << std::endl;
        std::cout << "manufacturer: " << manufacturerBase << std::endl;
        std::cout << "family: " << familyBase << std::endl;
        std::cout << "model: " << modelBase << std::endl;
        std::cout << "announcedDate: " << announcedDateBase << std::endl;
        std::cout << std::endl;

}//dump


