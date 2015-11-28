//
//  Vocabulary.cpp
//  inf555
//
//  Created by Camille MASSET on 27/11/2015.
//  Copyright © 2015 Basile Camille. All rights reserved.
//

#include "Vocabulary.hpp"

#include <random>
#include <chrono>
#include <cassert>

using namespace std;

float featuresDist2(array<float, Vocabulary::FEAT_SIZE> a, array<float, Vocabulary::FEAT_SIZE> b) {
    assert(a.size() == b.size());
    int s = a.size();
    
    float l = 0;
    for (int i = 0; i < s; i++) {
        l += (a[i] - b[i]) * (a[i] - b[i]);
    }
    
    return l;
}

Vocabulary::Vocabulary() {
    this->size = 0;
    this->MSE = 0.;
    this->kMeansDone = false;
}

Vocabulary::Vocabulary(int s, vector<array<float, FEAT_SIZE>> const &feats) {
    this->size = s;
    this->features = feats;
    this->MSE = 0.;
    this->kMeansDone = false;
    
    // Initialize centroids
    unsigned seed = unsigned(std::chrono::system_clock::now().time_since_epoch().count());
    default_random_engine gen(seed);
    uniform_int_distribution<double> unif_distr(0, feats.size());
    for (int i = 0; i < s; i++) {
        int j = unif_distr(gen);
        this->centroids.push_back(features[j]);
    }
    
    // Compute initial MSE
    for (vector<array<float, FEAT_SIZE>>::iterator it = this->features.begin(); it != this->features.end(); ++it) {
        float l = featuresDist2(*it, this->centroids.front());
        int i0 = 0;
        for (int i = 0; i < this->size; i++) {
            float ll = featuresDist2(*it, this->centroids[i]);
            if (ll < l) {
                l = ll;
                i0 = i;
            }
        }
        MSE += l;
    }
}

void Vocabulary::kMeans() {
    int s = this->size;
    float MSEa = 0.;
    while (MSEa != this->MSE) {
        MSEa = this->MSE;
        float MSEp = 0.;
        vector<array<float, FEAT_SIZE>> new_centroids;
        int classes_sizes[s];
        for (int i = 0; i < s; i++) {
            array<float, FEAT_SIZE> cent = {};
            new_centroids.push_back(cent);
            classes_sizes[i] = 0;
        }
        
        // Find the closest centroid for each feature
        for (vector<array<float, FEAT_SIZE>>::iterator it = features.begin(); it != features.end(); ++it) {
            float l = featuresDist2(*it, centroids[0]);
            int i0 = 0;
            for (int i = 0; i < s; i++) {
                float ll = featuresDist2(*it, centroids[i]);
                if (ll < l) {
                    l = ll;
                    i0 = i;
                }
            }
            for (int j = 0; j < FEAT_SIZE; j++) {
                new_centroids[i0][j] += (*it)[j];
            }
            classes_sizes[i0] += 1;
            MSEp += l;
        }
        
        // Update centroids
        for (int i = 0; i < s; i++) {
            classes_sizes[i] = max(classes_sizes[i], 1);
            for (int j = 0; j < FEAT_SIZE; j++) {
                centroids[i][j] = new_centroids[i][j] / classes_sizes[i];
            }
        }
        
        // Update MSE
        MSE = MSEp;
    }
    
    kMeansDone = true;
}




