//
//  DistributedViews.cpp
//  inf555
//
//  Created by Camille MASSET on 17/11/2015.
//  Copyright © 2015 Basile Camille. All rights reserved.
//

#include "DistributedViews.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <fstream>

using namespace std;

DistributedViews::DistributedViews(int d_) {
    d = d_;
    MSE = 0.;
    centroids = new Point3[d_];
    // créer un maillage de la sphère unité --> vertices
    std::string filename = "/Users/ntag/Documents/X/2015-2016/INF555/inf555/sphere2.off";
    initSphere(filename);
}

int DistributedViews::getNum() const {
    return this->d;
}


void DistributedViews::initSeeds() {
    MSE = 0.;
    // choisir this.d sommets dans le maillage et les places dans this.centroids
    srand(time(NULL));
    int d = this->getNum();
    int indexes[d];
    for (int i = 0; i < d; i++) {
        indexes[i] = rand() % vertices.size(); // TODO - Check that indexes are unique
    }
    for (int i = 0; i < d; i++) {
        centroids[i] = vertices[indexes[i]];
    }
    
    // calculer MSE
    for (vector<Point3>::iterator it = vertices.begin(); it != vertices.end(); ++it) {
        double l = it->distance2From(centroids[0]);
        int i0 = 0;
        for (int i = 0; i < d; i++) {
            double ll = it->distance2From(centroids[i]);
            if (ll < l) {
                l = ll;
                i0 = i;
            }
        }
        MSE += l;
    }
}

Point3* DistributedViews::getDirections() {
    initSeeds();
    int d = this->getNum();
    double MSEa = 0;
    while (MSEa != MSE) {
        MSEa = MSE;
        double MSEp = 0.;
        Point3* new_centroids = new Point3[d];
        int classes_sizes[d];
        for (int i = 0; i < d; i++) {
            new_centroids[i] = Point3();
            classes_sizes[i] = 0;
        }
        
        // We find the closest centroid for each vertex
        for (vector<Point3>::iterator it = vertices.begin(); it != vertices.end(); ++it) {
            double l = it->distance2From(centroids[0]);
            int i0 = 0;
            for (int i = 0; i < d; i++) {
                double ll = it->distance2From(centroids[i]);
                if (ll < l) {
                    l = ll;
                    i0 = i;
                }
            }
            new_centroids[i0] = new_centroids[i0] + (*it);
            classes_sizes[i0] += 1;
            MSEp += l;
        }
        
        for (int i = 0; i < d; i++) {
            classes_sizes[i] = max(classes_sizes[i], 1);
            centroids[i] = new_centroids[i] * (1./classes_sizes[i]);
        }
        
        MSE = MSEp;
    }
    
    return centroids;
}

void DistributedViews::initSphere(std::string filename) {
    ifstream File(filename);
    string Line;
    string Name;
    
    while(getline(File, Line)) {
        float* Vertex = new float[3];
        sscanf(Line.c_str(), "%f %f %f", &Vertex[0], &Vertex[1], &Vertex[2]);
        Point3 p(Vertex[0], Vertex[1], Vertex[2]);
        vertices.push_back(p);
    }

}