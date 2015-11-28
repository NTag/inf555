//
//  Histogram.cpp
//  inf555
//
//  Created by Camille MASSET on 26/11/2015.
//  Copyright © 2015 Basile Camille. All rights reserved.
//

#include "Histogram.hpp"
#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <math.h>

using namespace std;

Histogram::Histogram(int* counts, int vocab_size, double* freq, int coll_size) {
    this->size = vocab_size;
    
    double s = 0;
    for (int i = 0; i < vocab_size; i++) {
        s += counts[i];
    }

    for (int i = 0; i < vocab_size; i++) {
        if (freq[i] == 0) {
            this->coords.push_back(0);
        } else {
            this->coords.push_back((counts[i] / s) * log10(coll_size / freq[i]));
        }
    }
}
Histogram::Histogram(double* coords, int vocab_size) {
    this->size = vocab_size;
    for (int i = 0; i < vocab_size; i++) {
        this->coords.push_back(coords[i]);
    }
    cout << this->norm() << endl;
}

Histogram::~Histogram() {
    // delete[] this->coords;
}

double Histogram::norm() const {
    double n = 0;
    for (int i = 0; i < this->size; i++) {
        n += coords[i] * coords[i];
    }
    
    return sqrt(n);
}

double Histogram::similarity(const Histogram &a) const {
    assert(this->size == a.size);
    double thisNorm = this->norm();
    double aNorm = a.norm();
    assert(thisNorm > 0);
    assert(aNorm > 0);
    
    double s = 0;
    for (int i = 0; i < this->size; i++) {
        s += this->coords[i] * a.coords[i];
    }
    
    return s / (thisNorm * aNorm);
}