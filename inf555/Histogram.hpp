//
//  Histogram.hpp
//  inf555
//
//  Created by Camille MASSET on 26/11/2015.
//  Copyright © 2015 Basile Camille. All rights reserved.
//

#ifndef Histogram_hpp
#define Histogram_hpp

#include <stdio.h>
#include <vector>

class Histogram {
public:
    int size;
    std::vector<double> coords;
    Histogram(int* counts, int vocab_size, double* freq, int coll_size);
    Histogram(double* coords, int vocab_size);
    ~Histogram();
    double norm() const;
    double similarity(Histogram const &a) const;
};

#endif /* Histogram_hpp */
