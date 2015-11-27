//
//  HistogramHelper.cpp
//  inf555
//
//  Created by Basile Bruneau on 27/11/2015.
//  Copyright © 2015 Basile Camille. All rights reserved.
//

#include "HistogramHelper.hpp"

HistogramHelper::HistogramHelper(int wd, vector<float*> *words) {
    this->words = words;
    this->lenthOfWords = wd;
    this->numberOfWords = this->words->size();
    this->frequences = new double[this->numberOfWords];
    for (int i = 0; i < this->numberOfWords; i++) {
        this->frequences[i] = 0;
    }
}
float HistogramHelper::distanceBetweenFeatures(float *w1, float *w2) const {
    float distance = 0;
    for (int i = 0; i < this->lenthOfWords; i++) {
        distance = distance + w1[i]*w2[i];
    }
    return distance;
}
int HistogramHelper::findClosestWord(float *feature) {
    float dmin = -1;
    int imin = -1;
    
    for (int i = 0; i < this->numberOfWords; i++) {
        float cd = this->distanceBetweenFeatures(feature, this->words->at(i));
        if (dmin < 0 or cd < dmin) {
            dmin = cd;
            imin = i;
        }
    }
    
    return imin;
}

int HistogramHelper::addPreHistogram(string name) {
    int* preh = new int[this->numberOfWords];
    
    this->prehistograms->push_back(preh);
    this->names->push_back(name);
    
    return this->prehistograms->size() - 1;
}
void HistogramHelper::addFeatureForPreHistogram(int i, float *feature) {
    int closestWord = findClosestWord(feature);
    this->prehistograms[i][closestWord]++;
    
    // On met à jour dans la foulée les fréquences
    this->frequences[closestWord]++;
}

void HistogramHelper::computeFrequences() {
    for (int i = 0; i < this->numberOfWords; i++) {
        this->frequences[i] = this->frequences[i] / this->numberOfWords;
    }
}
void HistogramHelper::computeHistograms() {
    this->computeFrequences();
    int numberOfHistograms = this->prehistograms->size();
    
    while (!this->prehistograms->empty()) {
        int* cpreh = this->prehistograms->back();
        
        this->histograms->push_back(new Histogram(cpreh, this->numberOfWords, this->frequences, numberOfHistograms));
        
        this->prehistograms->pop_back();
        delete[] cpreh;
    }
}

bool HistogramHelper::saveHistograms(string filename) {
    ofstream file;
    file.open(filename);
    
    int numberOfHistograms = this->prehistograms->size();
    
    // Première ligne : numbre-de-mots longueur-d'un-mot nombre-de-vues
    file << this->numberOfWords << " " << this->lenthOfWords << numberOfHistograms << "\n";
    
    // Ensuite on stocke les mots : index coordonnées
    for (int i = 0; i < this->numberOfWords; i++) {
        file << i;
        for (int j = 0; j < this->lenthOfWords; j++) {
            file << " " << this->words->at(i)[j];
        }
        file << "\n";
    }
    
    // Puis enfin on stocke les histogrammes
    for (int i = 0; i < numberOfHistograms; i++) {
        file << this->names->at(i);
        for (int j = 0; j < numberOfWords; j++) {
            file << " " << this->histograms->at(i)->coords[j];
        }
        file << "\n";
    }
    
    file.close();
    
    return true;
}