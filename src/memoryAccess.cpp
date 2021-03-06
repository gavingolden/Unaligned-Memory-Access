/**
   Copyright (c) 2015 Gavin Golden gavinegolden@gmail.com.
   All rights reserved.
   
   A simple program to test the efficiency of sequential (un)aligned word
   array access patterns.
   
   A series of tests has shown that runtime does not differ for sequential
   memory access with non-word alignment.
   
   I believe this is due to block caching that effectively negates any
   "advantages" that aligned access presents because cache misses will occur
   at the same rate for all offsets but on slightly different iterations
   of the loop.
*/


#include <iostream>
#include <algorithm>
#include <typeinfo>
#include <vector>
#include "gTimer.h"


#define ARR_SIZE 10000
#define DESIRED_OFFSET 4

/** Data type to be used for the test. */
typedef unsigned ValType;

/** Hold random data to access aligned/unaligned memory locations. */
std::vector<ValType> data(ARR_SIZE);

#ifdef RANDOM
    /** Hold a list of randomly generated index values. */
    std::vector<unsigned long> randIndices(ARR_SIZE);
#endif

/** Fill vector with random values. */
void initData() {
    std::for_each(data.begin(), data.end(), [](ValType & ui) {
        ui = rand() % 2;
    });
    #ifdef RANDOM
    std::for_each(randIndices.begin(), randIndices.end(), [](unsigned long& num) {
        num = rand() % ARR_SIZE;
    });
    #endif
}



/** Driver method.
 */
int main(int argc, const char * argv[]) {
    srand(1);

    #ifdef RANDOM
    std::cout << "----- RANDOM ACCESS ------" << std::endl;
    #else
    std::cout << "----- SEQUENTIAL ACCESS ------" << std::endl;
    #endif

    /** Repeat the summation loop on a smallish vector rather than
        creating an enormous vector */
    const unsigned REPS = (argc == 2 ? atoi(argv[1]) : 10000);
    /** Cannot offset by a size greater than the num bytes in #ValType. */
    const int MAX_OFFSET = std::min(DESIRED_OFFSET, (int)sizeof(typeid(ValType)));

    // Initialize array with random values
    initData();

    // Perform some work so that the compiler doesn't do away with loops.
    unsigned long long sum = 0;

    // Perform array addition for various offsets beginning with zero
    for (int offset = 0; (offset < MAX_OFFSET); offset++) 
    {
        ValType* curr = reinterpret_cast<ValType*>(reinterpret_cast<char*>(data.data()) + offset);
        std::cout << "Start address for offset [" << offset << "]: " << curr << std::endl;

        GUtil::Timer timer;
        timer.start();
        for (size_t rep = 0; (rep < REPS); rep++) 
        {
            curr = reinterpret_cast<ValType*>(reinterpret_cast<char*>(data.data()) + offset);

            for (size_t i = 0; (i < ARR_SIZE - 1); i++) 
            {
                #ifdef RANDOM
                sum += *(curr + randIndices[i]);
                #else
                // Uncomment to prevent wonky -O3 optimization segfault
                /*
                if (curr > &(*(data.end()--))) {
                    std::cerr << "Invalid address." << std::endl;
                    return 1;
                }
                */
                sum += (*curr);
                curr++;
                #endif
            }
        }

        timer.end();
        std::cout << "Time --> " << timer << "\n\n";
    }
    std::cout << "Sum : " << sum << std::endl;
    return 0;
}
