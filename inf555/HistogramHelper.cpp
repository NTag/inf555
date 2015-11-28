//
//  HistogramHelper.cpp
//  inf555
//
//  Created by Basile Bruneau on 27/11/2015.
//  Copyright © 2015 Basile Camille. All rights reserved.
//

#include "HistogramHelper.hpp"
#include <iostream>

using namespace std;

HistogramHelper::HistogramHelper(int wd, vector<array<float, FEAT_SIZE>> words) {
    this->words = words;
    this->lengthOfWords = wd;
    this->numberOfWords = this->words.size();
    this->frequences = new double[this->numberOfWords];
    this->numberOfFeatures = 0;
    for (int i = 0; i < this->numberOfWords; i++) {
        this->frequences[i] = 0;
    }
}


HistogramHelper::HistogramHelper(string filename) {
    ifstream File(filename);
    string Line;
    string Name;
    
    int l = 0;
    int numberOfHistograms;
    while(getline(File, Line)) {
        l++;
        
        if (l == 1) { // On lit le nombre de mots, la longueur des mots et le nombre d'histogrammes
            int* Numbers = new int[3];
            sscanf(Line.c_str(), "%d %d %d", &Numbers[0], &Numbers[1], &Numbers[2]);
            this->numberOfWords = Numbers[0];
            this->lengthOfWords = Numbers[1];
            numberOfHistograms = Numbers[2];
            this->frequences = new double[this->numberOfWords];
        } else if (l - 1 <= this->numberOfWords) { // On lit les coordonnées des mots
            istringstream split(Line);
            string each;
            
            array<float, FEAT_SIZE> currentWord = {};
            
            int j = 0;
            while (getline(split, each, ' ')) {
                istringstream conv(each);
                double x;
                if (!(conv >> x)) {
                    continue;
                }
                if (j == 0) { // La ligne commence par la fréquence
                    this->frequences[l-2] = x;
                } else {
                    currentWord[j-1] = x;
                }
                j++;
            }
            
            this->words.push_back(currentWord);
        } else if (l - 1 - this->numberOfWords < numberOfHistograms) { // On lit des histogrammes
            istringstream split(Line);
            string each;
            
            int j = 0;
            while (getline(split, each, ' ')) {
                if (j == 0) {
                    this->names.push_back(each);
                } else {
                    double* currentHistogram = new double[this->numberOfWords];
                    
                    istringstream conv(each);
                    double x;
                    if (!(conv >> x)) {
                        continue;
                    }
                    currentHistogram[j] = x;
                    
                    this->histograms.push_back(new Histogram(currentHistogram, this->numberOfWords));
                }
                j++;
            }
        }
    }
}


float HistogramHelper::distanceBetweenFeatures(array<float, FEAT_SIZE> w1, array<float, FEAT_SIZE> w2) const {
    float distance = 0;
    for (int i = 0; i < this->lengthOfWords; i++) {
        distance = distance + (w1[i] - w2[i]) * (w1[i] - w2[i]);
    }
    return distance;
}


int HistogramHelper::findClosestWord(array<float, FEAT_SIZE> feature) {
    float dmin = -1;
    int imin = -1;
    
    for (int i = 0; i < this->numberOfWords; i++) {
        float cd = this->distanceBetweenFeatures(feature, this->words[i]);
        if (dmin < 0 or cd < dmin) {
            dmin = cd;
            imin = i;
        }
    }
    
    return imin;
}


int HistogramHelper::addPreHistogram(string name) {
    int* preh = new int[this->numberOfWords];
    for (int i = 0; i < this->numberOfWords; i++) {
        preh[i] = 0;
    }
    
    this->prehistograms.push_back(preh);
    this->names.push_back(name);
    
    return this->prehistograms.size() - 1;
}


void HistogramHelper::addFeatureForPreHistogram(int i, array<float, FEAT_SIZE> feature) {
    int closestWord = findClosestWord(feature);
    this->prehistograms[i][closestWord]++;
    
    // On met à jour dans la foulée les fréquences
    this->frequences[closestWord]++;
    this->numberOfFeatures++;
}


void HistogramHelper::computeFrequences() {
    for (int i = 0; i < this->numberOfWords; i++) {
        cout << this->frequences[i] << " ";
        this->frequences[i] = this->frequences[i] / this->numberOfFeatures;
        cout << this->frequences[i] << ";";
    }
    cout << endl;
}


void HistogramHelper::computeHistograms() {
    this->computeFrequences();
    int numberOfHistograms = this->prehistograms.size();
    vector<string> newNames;
    
    while (!this->prehistograms.empty()) {
        int* cpreh = this->prehistograms.back();
        
        this->histograms.push_back(new Histogram(cpreh, this->numberOfWords, this->frequences, numberOfHistograms));
        newNames.push_back(this->names.back());
        
        this->prehistograms.pop_back();
        this->names.pop_back();
        delete[] cpreh;
    }
    
    this->names = newNames;
}

vector<string> HistogramHelper::findClosestModels(vector<array<float, FEAT_SIZE>> features, int numberOfResults) {
    // On calcule les counts
    int* counts = new int[this->numberOfWords];
    for (int i = 0; i < this->numberOfWords; i++) {
        counts[i] = 0;
    }
    for (int i = 0; i < features.size(); i++) {
        int closestWord = findClosestWord(features[i]);
        counts[closestWord]++;
    }
    
    // On génère l'histogramme
    Histogram h(counts, this->numberOfWords, this->frequences, this->histograms.size());
    
    return this->findClosestModels(h, numberOfResults);
}

vector<string> HistogramHelper::findClosestModels(Histogram &h, int numberOfResults) {
    vector<string> results;
    int numberOfHistograms = this->histograms.size();
    
    double* d = new double[numberOfHistograms];
    for (int i = 0; i < numberOfHistograms; i++) {
        d[i] = this->histograms[i]->similarity(h);
    }
    
    // Algorithme surement lent mais simple
    for (int i = 0; i < numberOfResults; i++) {
        double dmin = -1;
        int jmin = -1;
        
        for (int j = 0; j < numberOfHistograms; j++) {
            if (dmin < 0 or (d[j] < dmin and d[j] >= 0)) {
                dmin = d[j];
                jmin = j;
            }
        }
        results.push_back(this->names[jmin]);
        d[jmin] = -1;
    }
    
    return results;
}


bool HistogramHelper::saveHistograms(string filename) {
    ofstream file;
    file.open(filename);
    
    int numberOfHistograms = this->histograms.size();
    
    // Première ligne : numbre-de-mots longueur-d'un-mot nombre-de-vues
    file << this->numberOfWords << " " << this->lengthOfWords << " " << numberOfHistograms << "\n";
    
    // Ensuite on stocke les mots : fréquence coordonnées
    for (int i = 0; i < this->numberOfWords; i++) {
        file << this->frequences[i];
        for (int j = 0; j < this->lengthOfWords; j++) {
            file << " " << this->words[i][j];
        }
        file << "\n";
    }
    
    // Puis enfin on stocke les histogrammes nom valeurs
    for (int i = 0; i < numberOfHistograms; i++) {
        file << this->names[i];
        for (int j = 0; j < numberOfWords; j++) {
            file << " " << this->histograms[i]->coords[j];
        }
        file << "\n";
    }
    
    file.close();
    
    return true;
}