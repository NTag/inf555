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
#include <array>

class Vocabulary {
public:
    static const int FEAT_SIZE = 256;
    int size;
    std::vector<std::array<float, FEAT_SIZE>> centroids;
    std::vector<std::array<float, FEAT_SIZE>> features;
    float MSE;
    bool kMeansDone;
    
    Vocabulary();
    Vocabulary(int s, std::vector<std::array<float, FEAT_SIZE>> const &feats);
    void kMeans();
};

#endif /* Vocabulary_hpp */
