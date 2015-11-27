//
//  Vocabulary.hpp
//  inf555
//
//  Created by Camille MASSET on 27/11/2015.
//  Copyright © 2015 Basile Camille. All rights reserved.
//

#ifndef Vocabulary_hpp
#define Vocabulary_hpp

#include <stdio.h>
#include <vector>

class Vocabulary {
public:
    int size;
    int feat_long;
    float** centroids;
    std::vector<float*> features;
    float MSE;
    bool kMeansDone;
    
    Vocabulary();
    Vocabulary(int s, std::vector<float*> const &feats, int f_long);
    ~Vocabulary();
    void kMeans();
};

#endif /* Vocabulary_hpp */
