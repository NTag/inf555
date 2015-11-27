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

using namespace std;

float featuresDist2(float* a, float* b, int s) {
    float l = 0;
    for (int i = 0; i < s; i++) {
        l += (a[i] * b[i]) * (a[i] * b[i]);
    }
    
    return l;
}

Vocabulary::Vocabulary(int s, vector<float*> const &feats, int f_long) {
    this->size = s;
    this->feat_long = f_long;
    this->features = feats;
    this->MSE = 0.;
    this->centroids = new float*[s];
    this->kMeansDone = false;
    
    // Initialize centroids
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine gen(seed);
    uniform_int_distribution<double> unif_distr(0, feats.size());
    for (int i = 0; i < s; i++) {
        int j = unif_distr(gen);
        this->centroids[i] = features[j];
    }
    
    // Compute initial MSE
    for (vector<float*>::iterator it = this->features.begin(); it != this->features.end(); ++it) {
        float l = featuresDist2(*it, this->centroids[0], f_long);
        int i0 = 0;
        for (int i = 0; i < this->size; i++) {
            float ll = featuresDist2(*it, this->centroids[i], f_long);
            if (ll < l) {
                l = ll;
                i0 = i;
            }
        }
        MSE += l;
    }
}

Vocabulary::~Vocabulary() {
    for (int i = 0; i < this->size; i++) {
        delete[] this->centroids[i];
    }
    delete[] this->centroids;
//    
//    for (vector<float*>::iterator it = this->features.begin(); it != this->features.end(); ++it) {
//        delete[] (*it);
//    }
}

void Vocabulary::kMeans() {
    int s = this->size;
    float MSEa = 0.;
    while (MSEa != this->MSE) {
        MSEa = this->MSE;
        float MSEp = 0.;
        float** new_centroids = new float*[s];
        int classes_sizes[s];
        for (int i = 0; i < s; i++) {
            new_centroids[i] = new float[feat_long];
            classes_sizes[i] = 0;
        }
        
        // Find the closest centroid for each feature
        for (vector<float*>::iterator it = features.begin(); it != features.end(); ++it) {
            float l = featuresDist2(*it, centroids[0], feat_long);
            int i0 = 0;
            for (int i = 0; i < s; i++) {
                float ll = featuresDist2(*it, centroids[i], feat_long);
                if (ll < l) {
                    l = ll;
                    i0 = i;
                }
            }
            for (int j = 0; j < feat_long; j++) {
                new_centroids[i0][j] += (*it)[j];
            }
            classes_sizes[i0] += 1;
            MSEp += l;
        }
        
        // Update centroids
        for (int i = 0; i < s; i++) {
            classes_sizes[i] = max(classes_sizes[i], 1);
            for (int j = 0; j < feat_long; j++) {
                centroids[i][j] = new_centroids[i][j] / classes_sizes[i];
            }
        }
        
        // Update MSE
        MSE = MSEp;
    }
    
    kMeansDone = true;
}




