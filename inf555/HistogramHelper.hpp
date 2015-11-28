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
#include <array>
#include <string>
#include <fstream>
#include <sstream>
#include "Histogram.hpp"

class HistogramHelper {
public:
    static const int FEAT_SIZE = 256;
    HistogramHelper(int wd, std::vector<std::array<float, FEAT_SIZE>> words); // wd : longueur des mots
    HistogramHelper(std::string filename); // Load histograms from a file
    
    int findClosestWord(std::array<float, FEAT_SIZE> feature);
    float distanceBetweenFeatures(std::array<float, FEAT_SIZE> w1, std::array<float, FEAT_SIZE> w2) const;
    
    int addPreHistogram(std::string name); // Ajouter un nouveau pre-histogramme (une nouvelle vue) - retourne son index
    void addFeatureForPreHistogram(int i, std::array<float, FEAT_SIZE> feature); // Prendre en compte une nouvelle feature pour la vue numéro i
    
    void computeFrequences();
    void computeHistograms(); // Calculer les histogrammes et supprimer les pré-histogrammes
    
    std::vector<std::string> findClosestModels(std::vector<std::array<float, FEAT_SIZE>> features, int numberOfResults);
    std::vector<std::string> findClosestModels(Histogram &h, int numberOfResults);
    
    bool saveHistograms(std::string filename);
    std::vector<Histogram*> histograms; // Histogrammes des différents modèles
    
private:
    int lengthOfWords; // Longueur des mots et des features
    int numberOfWords; // Nombre de mots
    std::vector<std::array<float, FEAT_SIZE>> words;
    std::vector<int*> prehistograms; // Préhistogrammes : pas normalisés en tenant compte de la fréquence
    double* frequences; // Fréquences des mots
    std::vector<std::string> names;
    int numberOfFeatures;
};

#endif /* HistogramHelper_hpp */
