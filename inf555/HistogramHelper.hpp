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
#include <fstream>
#include <sstream>
#include "Histogram.hpp"

class HistogramHelper {
public:
    HistogramHelper(int wd, std::vector<float*> words); // wd : longueur des mots
    HistogramHelper(std::string filename); // Load histograms from a file
    
    int findClosestWord(float* feature);
    float distanceBetweenFeatures(float* w1, float* w2) const;
    
    int addPreHistogram(std::string name); // Ajouter un nouveau pre-histogramme (une nouvelle vue) - retourne son index
    void addFeatureForPreHistogram(int i, float* feature); // Prendre en compte une nouvelle feature pour la vue numéro i
    
    void computeFrequences();
    void computeHistograms(); // Calculer les histogrammes et supprimer les pré-histogrammes
    
    std::vector<std::string> findClosestModels(std::vector<float*> features, int numberOfResults);
    std::vector<std::string> findClosestModels(Histogram &h, int numberOfResults);
    
    bool saveHistograms(std::string filename);
    std::vector<Histogram*> histograms; // Histogrammes des différents modèles
    
private:
    int lengthOfWords; // Longueur des mots et des features
    int numberOfWords; // Nombre de mots
    std::vector<float*> words;
    std::vector<int*> prehistograms; // Préhistogrammes : pas normalisés en tenant compte de la fréquence
    double* frequences; // Fréquences des mots
    std::vector<std::string> names;
    int numberOfFeatures;
};

#endif /* HistogramHelper_hpp */
