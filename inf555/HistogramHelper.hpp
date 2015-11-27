//
//  HistogramHelper.hpp
//  inf555
//
//  Created by Basile Bruneau on 27/11/2015.
//  Copyright © 2015 Basile Camille. All rights reserved.
//

#ifndef HistogramHelper_hpp
#define HistogramHelper_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include "Histogram.hpp"

using namespace std;

class HistogramHelper {
public:
    HistogramHelper(int wd, vector<float*>* words); // wd : longueur des mots
    
    int findClosestWord(float* feature);
    float distanceBetweenFeatures(float* w1, float* w2) const;
    
    int addPreHistogram(string name); // Ajouter un nouveau pre-histogramme (une nouvelle vue) - retourne son index
    void addFeatureForPreHistogram(int i, float* feature); // Prendre en compte une nouvelle feature pour la vue numéro i
    
    void computeFrequences();
    void computeHistograms(); // Calculer les histogrammes et supprimer les pré-histogrammes
    
    vector<Histogram*>* histograms; // Histogrammes des différents modèles
    
private:
    int lenthOfWords; // Longueur des mots et des features
    int numberOfWords; // Nombre de mots
    vector<float*>* words;
    vector<int*>* prehistograms; // Préhistogrammes : pas normalisés en tenant compte de la fréquence
    double* frequences; // Fréquences des mots
    vector<string>* names;
};

#endif /* HistogramHelper_hpp */
