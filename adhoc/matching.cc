/*
Snapsort-Challenge -- An answer to the Snapsort coding challenge
Written by Chris Mennie (chris at chrismennie.ca or cmennie at rogers.com)
Copyright (C) 2011 Chris A. Mennie                              

License: Released under the GPL version 3 license. See the included LICENSE.
*/


#include "adhoc.h"
#include "../datas.h"
#include "../listing.h"
#include "../product.h"
#include <iostream>
#include <stack>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/thread.hpp>


//To bridge boost mem_fn and tr1 -- can't be in the anonymous namespace
template<class T> const T* get_pointer(const std::tr1::shared_ptr<T>& ptr) 
{
    return ptr.get();
}

template<class T> T* get_pointer(std::tr1::shared_ptr<T>& ptr)
{
    return ptr.get();
}


namespace
{

//This is a helper for the worker threads. We synchronize access to the product stack work through here.
class ProductStack
{
    std::stack<std::tr1::shared_ptr<Product> > productStack;
    unsigned int processingCount;
    unsigned int origCount;
    boost::mutex productLock;

public:
    ProductStack(std::pair<std::vector<std::tr1::shared_ptr<Product> >::iterator, std::vector<std::tr1::shared_ptr<Product> >::iterator> productsPair) 
    {
        BOOST_FOREACH (std::tr1::shared_ptr<Product> product, productsPair) {
            productStack.push(product);
        }//foreach

        processingCount = 0;
        origCount = productStack.size();
    }//constructor

    //Safely grab the next product, pop it from the stack. Returns null if stack is empty.
    std::tr1::shared_ptr<Product> getNextProduct()
    {
        boost::mutex::scoped_lock lock(productLock);

        if (productStack.empty() == true) {
            return std::tr1::shared_ptr<Product>();
        }//if

        std::tr1::shared_ptr<Product> nextProduct = productStack.top();
        productStack.pop();

        if ((processingCount % 50) == 0) {
            std::cout << "Working on " << processingCount + 1 << " of " << origCount << std::endl;
        }//if
        ++processingCount;

        return nextProduct;
    }//getNextProduct
};//ProductStack

//Helper struct for scoring up words in product manufacturer
struct MatchInfo
{
    MatchInfo()
    {
        substringMatchAmount = 0.0f;
        matchedPosition = 0;
        diffPositionFromOriginal = 0;
        matchedPairDistanceDelta = 0;
        positionalMatchBonus = 0;
        isMatched = false;
    }//constructor

    float substringMatchAmount;    //If a partial match, how much of the word was matched
    unsigned int matchedPosition;  //What word position in the listing data was the match found at
    int matchedPairDistanceDelta;  //For a matched word to the next matched word, how far apart were they in the listing data
    int diffPositionFromOriginal;  //If we cared about the exact phrasing / ordering, how close were we to it
    float positionalMatchBonus;    //Used to weight score based on how close the position of the word was in the listing data 
                                   //to where it was in the position data
    bool isMatched;                //Did we match the product word at all
};//MatchInfo

enum FilterMode
{
    Manufacturer,
    Model,
    Family
};//FilterMode

//For each word from the product values, try to match it against the listing values,
//keeping info about the result in matchInfos
void fillMatchInfos(std::vector<MatchInfo> &matchInfos, std::vector<unsigned int> &productValues, std::vector<unsigned int> &listingData,
                        std::vector<bool> &matchedListingWords, FilterMode filterMode, StringTable &table)
{
    for (std::vector<unsigned int>::iterator productWordIter = productValues.begin(); productWordIter != productValues.end(); ++productWordIter) {
        MatchInfo matchInfo;
        std::string &productWordStr = table.getString(*productWordIter);

        for (std::vector<unsigned int>::iterator listingWordIter = listingData.begin(); listingWordIter != listingData.end(); ++listingWordIter) {
            std::string &listingWordStr = table.getString(*listingWordIter);

            if (listingWordStr.size() < productWordStr.size()) {
                continue;
            }//if

            //Is the product word at the start or end of the listing word?            
            //Note: We only allow for partial matches with the manufacturer. Exact matching on the model/family worked much better.
            //      Partial matches are only considered at the beginning and end of the listing word
            bool fullMatch = (listingWordStr == productWordStr);
            bool partialMatch = ((listingWordStr.substr(0, productWordStr.size()) == productWordStr) || 
                (listingWordStr.substr(listingWordStr.size() - productWordStr.size(), productWordStr.size()) == productWordStr));
            
            if ( ((Manufacturer == filterMode) && (true == partialMatch)) ||
                 (true == fullMatch) ) {
                matchInfo.isMatched = true;
                matchInfo.substringMatchAmount = ((float)productWordStr.size() / ((float)listingWordStr.size()));
                matchInfo.matchedPosition = std::distance(listingData.begin(), listingWordIter);
                matchInfo.diffPositionFromOriginal = matchInfo.matchedPosition - std::distance(productValues.begin(), productWordIter);

                matchedListingWords[matchInfo.matchedPosition] = true;

                if (Manufacturer != filterMode) {
                    break; //assume first match is the most significant
                }//if
            }//if
        }//for

        matchInfos.push_back(matchInfo);
    }//for

    int matchInfosSize = matchInfos.size();

    //Get a sense of the relative ordering of the words from the product to the listing
    for (int matchInfoPos = 0; matchInfoPos < matchInfosSize - 1; ++matchInfoPos) {
        if (false == matchInfos[matchInfoPos].isMatched) {
            continue;
        }//if

        for (int matchInfoPosInner = matchInfoPos + 1; matchInfoPosInner < matchInfosSize; ++matchInfoPosInner) {
            if (false == matchInfos[matchInfoPosInner].isMatched) {
                continue;
            }//if

            matchInfos[matchInfoPos].matchedPairDistanceDelta = matchInfos[matchInfoPosInner].matchedPosition - matchInfos[matchInfoPos].matchedPosition;
        }//for
    }//for

    if (matchInfos.empty() == false) {
        matchInfos[matchInfosSize - 1].matchedPairDistanceDelta = 0;
    }//if
}//fillMatchInfos

//Based on the position of a word, compute it's bonus via the harmonic series.
//Depending on the filter mode we either use a stright harmonic series or balance it from
//both ends of the matchInfos vector
float computePositionalMatchBonuses(std::vector<MatchInfo> &matchInfos, FilterMode filterMode)
{
    float maxScore = 0.0f;

    int matchInfosSize = matchInfos.size();

    if (matchInfosSize != 0) {
        switch (filterMode) {
            case Manufacturer:
                //Compute the weighted partial harmonic number for the exact matching weight portion
                for (int pos = 0; pos < matchInfosSize; ++pos) {
                    maxScore += 50.0f * (1.0f / ((float)(pos + 1.0f)));
                    matchInfos[pos].positionalMatchBonus = 50.0f * (1.0f / ((float)(pos + 1.0f)));
                }//for
                break;

            case Model:
                {
                //Compute the weighted "balanced" partial harmonic number for the exact matching weight portion
                float extraScore = 0.0f;
                for (int pos = 0; pos < matchInfosSize / 2; ++pos) {
                    extraScore += 50.0f * (1.0f / ((float)(pos + 1.0f)));
                    matchInfos[pos].positionalMatchBonus = 50.0f * (1.0f / ((float)(pos + 1.0f)));
                    matchInfos[matchInfosSize - pos - 1].positionalMatchBonus = 50.0f * (1.0f / ((float)(pos + 1.0f)));
                }//for

                extraScore *= 2.0f;

                if ((matchInfosSize % 2) == 1) {
                    extraScore += 50.0f * (1.0f / ((float)(matchInfosSize / 2 + 1)));
                    matchInfos[matchInfosSize / 2].positionalMatchBonus = 50.0f * (1.0f / ((float)(matchInfosSize / 2 + 1)));
                }//if            

                maxScore += extraScore;
                }
                break;

            case Family:
                ;
                //Nothing
        }//switch
    }//if

    return maxScore;
}//computePositionalMatchBonuses

//With positional bonuses in hand, apply them to the matched words.
//As well, we'll add in the substring match and word pair distance delta scores
float handOutScores(std::vector<MatchInfo> &matchInfos, FilterMode filterMode)
{
    float curScore = 0.0f;

    for (std::vector<MatchInfo>::iterator matchInfoIter = matchInfos.begin(); matchInfoIter != matchInfos.end(); ++matchInfoIter) {
        MatchInfo &matchInfo = *matchInfoIter;

        if (false == matchInfo.isMatched) {
            continue;
        }//if

        //Substring score
        curScore += 50.0f * matchInfo.substringMatchAmount;

        //Positional match bonus score
        if (Manufacturer == filterMode) {
            float isNeg = 1.0f;
            if (matchInfo.diffPositionFromOriginal < 0) {
                isNeg = -1.0f;
            }//if

            curScore +=  matchInfo.positionalMatchBonus * (1.0f / (((float)matchInfo.diffPositionFromOriginal) + 1.0f * isNeg));
            //Negatives are ok.. penalty for being out of order
        } else {
            //If we missed a word, we are already penalised by not adding this to the score
            curScore += matchInfo.positionalMatchBonus;
        }//if
 
        //Word pait distance delta score
        float isNeg = 1.0f;
        if (matchInfo.matchedPairDistanceDelta < 0) {
            isNeg = -1.0f;
        }//if

        curScore += 25.0f * (1.0f / (((float)matchInfo.matchedPairDistanceDelta) + 1.0f * isNeg));
    }//foreach

    return curScore;
}//handOutScores

//The common weight/score calculator. For a given set of product words and listing words (and mode), how well do they match?
float computeBaseWeight(std::vector<unsigned int> &productValues, std::vector<unsigned int> &listingData, FilterMode filterMode, StringTable &table)
{
    std::vector<MatchInfo> matchInfos;
    matchInfos.reserve(productValues.size());

    std::vector<bool> matchedListingWords;
    matchedListingWords.resize(listingData.size());

    //For each word from the product values, try to match it against the listing values
    fillMatchInfos(matchInfos, productValues, listingData, matchedListingWords, filterMode, table);

    //Compute weight
    float maxScore = (50.0f + 25.0f) * matchInfos.size();
    float curScore = 0.0f;

    maxScore += computePositionalMatchBonuses(matchInfos, filterMode);

    curScore += handOutScores(matchInfos, filterMode);

    //Penalties and normalization:

    //Unmatched penalties when looking at the model data
    if (Model == filterMode) {
        for (std::vector<MatchInfo>::iterator matchInfoIter = matchInfos.begin(); matchInfoIter != matchInfos.end(); ++matchInfoIter) {
            MatchInfo &matchInfo = *matchInfoIter;

            if (false == matchInfo.isMatched) {
                curScore -= matchInfo.positionalMatchBonus;
            }//if       
        }//for
    }//if

    //Normalize score
    if (maxScore > 0.0f) {
        curScore = curScore / maxScore;
    } else {
        curScore = 0.0f;
    }//if

    //4% penalty for each product word that wasn't matched
    for (std::vector<MatchInfo>::iterator matchInfoIter = matchInfos.begin(); matchInfoIter != matchInfos.end(); ++matchInfoIter) {
        MatchInfo &matchInfo = *matchInfoIter;

        if (false == matchInfo.isMatched) {
            curScore -= 0.04;
        }//if       
    }//for

    //We want to match the manufacturer exactly, so invoke harsh (10%) penalties for each listing manufacturer word not matched
    if (Manufacturer == filterMode) {
        unsigned int matchedCount = std::count_if(matchedListingWords.begin(), matchedListingWords.end(), std::bind2nd(std::equal_to<bool>(), true));
        float unmatchedWordsPenalty = ((float)(listingData.size() - matchedCount)) * 0.1f;

        curScore -= unmatchedWordsPenalty;
    }//if

    return curScore;
}//computeBaseWeight

//Compute the match weight/score of the product/listing values then add a normalized
//version of it to the existing score (common code, so we're adding chunks of the score 
//at a time)
void filter(std::vector<std::pair<std::tr1::shared_ptr<Listing>, float> > &filteredListings, 
            std::vector<unsigned int> &productValues, float categoryWeight, 
            boost::function<std::vector<unsigned int>&(std::tr1::shared_ptr<Listing>)> listingExtractionMethod,
            FilterMode filterMode,
            StringTable &table
            )
{
    std::vector<std::pair<std::tr1::shared_ptr<Listing>, float> > filteredListingsTmp;
    filteredListingsTmp.reserve(filteredListings.size());

    typedef std::pair<std::tr1::shared_ptr<Listing>, float> FilteredListingPair;
    BOOST_FOREACH (FilteredListingPair &filteredListing, filteredListings) {
        std::vector<unsigned int> &listingData = listingExtractionMethod(filteredListing.first);
        float baseWeight = computeBaseWeight(productValues, listingData, filterMode, table);

        //Normalize computed weight and then add it to the existing value
        float newWeight = filteredListing.second + baseWeight * categoryWeight;

        //Early filter of the listings to consider if we're looking at the 
        //manufacturer data
        if ((Manufacturer != filterMode) || (newWeight > 0.0f)) {
            filteredListingsTmp.push_back(std::make_pair(filteredListing.first, newWeight));
        }//if
    }//foreach

    filteredListings.swap(filteredListingsTmp);
}//filter

//Compute the portion of the final weight for comparing the manufacturer
void filterOnManufacturer(std::vector<std::pair<std::tr1::shared_ptr<Listing>, float> > &filteredListings, 
                            std::vector<unsigned int> &manufacturer, 
                            std::pair<std::vector<std::tr1::shared_ptr<Listing> >::iterator, std::vector<std::tr1::shared_ptr<Listing> >::iterator> listingsPair,
                            StringTable &table)
{
    //Fill filteredListings
    filteredListings.reserve(std::distance(listingsPair.first, listingsPair.second));

    BOOST_FOREACH (std::tr1::shared_ptr<Listing> listing, listingsPair) {
        filteredListings.push_back(std::make_pair(listing, 0.0f));
    }//foreach

    //Helper function to pass along to return the manufacturer data without knowing about it
    boost::function<std::vector<unsigned int>&(std::tr1::shared_ptr<Listing>)> listingExtractionMethod;
    listingExtractionMethod = boost::lambda::bind(boost::mem_fn(&Listing::getManufacturer), boost::lambda::_1);

    filter(filteredListings, manufacturer, 0.25, listingExtractionMethod, Manufacturer, table);
}//filterOnManufacturer

//Compute the portion of the final weight for comparing the model
void filterOnModel(std::vector<std::pair<std::tr1::shared_ptr<Listing>, float> > &filteredListings, 
                            std::vector<unsigned int> &model, StringTable &table)
{
    //Helper function to pass along to return the model data without knowing about it
    boost::function<std::vector<unsigned int>&(std::tr1::shared_ptr<Listing>)> listingExtractionMethod;
    listingExtractionMethod = boost::lambda::bind(boost::mem_fn(&Listing::getTitle), boost::lambda::_1);

    filter(filteredListings, model, 0.55, listingExtractionMethod, Model, table);
}//filterOnModel

//Compute the portion of the final weight for comparing the family
void filterOnFamily(std::vector<std::pair<std::tr1::shared_ptr<Listing>, float> > &filteredListings, 
                            std::vector<unsigned int> &family, StringTable &table)
{
    //Helper function to pass along to return the family data without knowing about it
    boost::function<std::vector<unsigned int>&(std::tr1::shared_ptr<Listing>)> listingExtractionMethod;
    listingExtractionMethod = boost::lambda::bind(boost::mem_fn(&Listing::getTitle), boost::lambda::_1);

    filter(filteredListings, family, 0.20, listingExtractionMethod, Family, table);
}//filterOnFamily

//Simple comparator
bool sortFilteredListingsComparator(std::pair<std::tr1::shared_ptr<Listing>, float> first, std::pair<std::tr1::shared_ptr<Listing>, float> second)
{
    return first.second > second.second;
}//sortFilteredListingsComparator

//Helper to sort filteredListings
void sortFilteredListings(std::vector<std::pair<std::tr1::shared_ptr<Listing>, float> > &filteredListings)
{
    std::sort(filteredListings.begin(), filteredListings.end(), sortFilteredListingsComparator);
}//sortFilteredListings

//Helper to sort the results
void sortResult(std::tr1::shared_ptr<ResultHolder> result)
{
    std::vector<std::pair<std::tr1::shared_ptr<Listing>, float> > results;
    for (unsigned int pos = 0; pos < result->getListings().size(); ++pos) {
        results.push_back(std::make_pair(result->getListings()[pos], result->getWeights()[pos]));
    }//for

    sortFilteredListings(results);

    for (unsigned int pos = 0; pos < results.size(); ++pos) {
        result->getListings()[pos] = results[pos].first;
        result->getWeights()[pos] = results[pos].second;
    }//for
}//sortResult

//After all the threads have completed, tell the products which listings we best matched with them.
//We then create the final set of results (subject to a final weight filtering later)
void productFinalResultsPreAcceptance(Datas &datas)
{
    //Note: we're not in a worker thread here

    //Tell products which listings are best suited for them
    BOOST_FOREACH (std::tr1::shared_ptr<Listing> listing, datas.getListingPair()) {
        if (listing->getBestMatchedProduct() == NULL) {
            continue;
        }//if

        listing->getBestMatchedProduct()->addMatchedListing(listing);
    }//foreach

    //Create result list
    BOOST_FOREACH (std::tr1::shared_ptr<Product> product, datas.getProductPair()) {
        std::tr1::shared_ptr<ResultHolder> newResult(new ResultHolder);
        newResult->setProduct(product);

        unsigned int numMatchedListings = std::distance(product->getMatchedListingPair().first, product->getMatchedListingPair().second);

        newResult->reserveListings(numMatchedListings);
        newResult->reserveWeights(numMatchedListings);

        BOOST_FOREACH (std::tr1::shared_ptr<Listing> listing, product->getMatchedListingPair()) {
            newResult->addListing(listing);
            newResult->addWeight(listing->getBestMatchedWeight());
        }//foreach

        //We don't need to, but let's sort the results by weight
        sortResult(newResult);

        datas.addResult(newResult);
    }//foreach
}//productFinalResultsPreAcceptance

//The real thread function. Applies scoring/filtering on the listing data for a single
//product, ultimately updating the listing with the better product matching (if found
//for the given listing and product). 
//The final product->listings mapping isn't done until after the threads have finished.
void determineListingsForProduct(Datas &datas, std::tr1::shared_ptr<Product> product)
{
    std::vector<std::pair<std::tr1::shared_ptr<Listing>, float> > filteredListings; //pair of (listing, weight)

    //Filter the list of listings a little and then compute weights for the ones that survive the cull
    filterOnManufacturer(filteredListings, product->getManufacturer(), datas.getListingPair(), datas.stringTable);
    filterOnModel(filteredListings, product->getModel(), datas.stringTable);
    filterOnFamily(filteredListings, product->getFamily(), datas.stringTable);

    //If this product is a better match for a listing, then update the listing to reflect that
    for (std::vector<std::pair<std::tr1::shared_ptr<Listing>, float> >::iterator filteredListingsIter = filteredListings.begin(); 
        filteredListingsIter != filteredListings.end(); ++filteredListingsIter) {

        std::tr1::shared_ptr<Listing> curListing = filteredListingsIter->first;

        {
        boost::mutex::scoped_lock lock(curListing->getListingLock());

        if (filteredListingsIter->second > curListing->getBestMatchedWeight()) {
            curListing->setBestMatchedProduct(product);
            curListing->setBestMatchedWeight(filteredListingsIter->second);
        }//if
        }
    }//for
}//determineListingsForProduct

//Thread worker function.. grab a product, match it up against all the listings.
//Repeat until no more products.
void workerThreadStart(std::tr1::shared_ptr<ProductStack> productStack, Datas &datas)
{
    std::tr1::shared_ptr<Product> curProduct = productStack->getNextProduct();
    while (curProduct != NULL) {
        determineListingsForProduct(datas, curProduct);

        //Product stack synchronizes the getter for us
        curProduct = productStack->getNextProduct();
    }//while
}//workerThreadStart

}//anonymous namespace

//Determine the product->listings matchings. Spawn off N threads and go from there.
void doAdhocMatching(Datas &datas, unsigned int numThreads)
{
    std::tr1::shared_ptr<ProductStack> productStack(new ProductStack(datas.getProductPair()));

    std::vector<std::tr1::shared_ptr<boost::function<void (void)> > > threadFuncPool;
    std::vector<std::tr1::shared_ptr<boost::thread> > threadPool;

    //Start threads
    for (unsigned int thread = 0; thread < numThreads; ++thread) {
        std::tr1::shared_ptr<boost::function<void (void)> > threadStartFunc(
                new boost::function<void (void)>(boost::lambda::bind(&workerThreadStart, boost::lambda::var(productStack), boost::lambda::var(datas)))
            );

        threadFuncPool.push_back(threadStartFunc);
        threadPool.push_back(std::tr1::shared_ptr<boost::thread>(new boost::thread(*threadStartFunc)));
    }//for

    //Wait for threads
    BOOST_FOREACH (std::tr1::shared_ptr<boost::thread> thread, threadPool) {
        thread->join();
    }//foreach

    //Complete the product->listings mappings
    productFinalResultsPreAcceptance(datas);

    std::cout << "Done!" << std::endl;
}//doAdhocMatching


