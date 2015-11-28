//
//  Galif.cpp
//  inf555
//
//  Created by Camille MASSET on 24/11/2015.
//  Copyright © 2015 Basile Camille. All rights reserved.
//

#include "Galif.hpp"

#include <time.h>
#include <random>
#include <iostream>
#include <chrono>

#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"


using namespace std;
using namespace cv;

int const FEAT_SIZE = 256;


GALIF::GALIF(double w0, double w, int k, int n) {
    assert(k > 0);
    assert(n > 0);
    
    this->w0 = w0;
    this->k = k;
    this->n = n;
    
    this->sigx = 0.02 * w;
    this->sigy = this->sigx / 0.3;
    
    this->filters = new Mat[k];
}

GALIF::~GALIF() {
    delete[] this->filters;
}

float GALIF::gaussian(int i, double u, double v, int cols, int rows) const {
    assert(0 <= i && i < this->k);
    
    double theta = i * M_PI / this->k;
   
    u = 2 * M_PI * u / cols;
    v = 2 * M_PI * v / rows;
    
    double ut = u * cos(theta) - v * sin(theta);
    double vt = u * sin(theta) + v * cos(theta);
    
    return float(exp(-2 * M_PI_2 * ((ut - this->w0)*(ut - this->w0) * this->sigx*this->sigx + vt*vt * this->sigy*this->sigy)));
}

Mat GALIF::get_filter(int i, int rows, int cols) const {
    assert(rows > 0 && cols > 0);
    
    Mat fi = Mat(rows, cols, CV_32F);
    for (int l = 0; l < cols; l++) {
        for (int j = 0; j < rows; j++) {
            fi.at<float>(Point(l, j)) = this->gaussian(i, l, j, cols, rows);
        }
    }
    
    return fi;
}

void GALIF::compute_filters(Mat const &I) {
    int rows = I.rows;
    int cols = I.cols;
    
    for (int i = 0; i < this->k; i++) {
        this->filters[i] = this->get_filter(i, rows, cols);
    }
}

Mat GALIF::filter(int i, cv::Mat const &I) const {
    assert(I.type() == CV_32FC1);
    
    // Paste the extanded image in a complex matrix
    Mat planes[] = { Mat_<float>(I), Mat::zeros(I.size(), CV_32F) };
    Mat complexI;
    merge(planes, 2, complexI);
    // Compute its DFT
    dft(complexI, complexI);
    
    // Compute point-wise multiplication by the gaussian filter
    Mat planesm[] = { Mat::zeros(complexI.size(), CV_32F), Mat::zeros(complexI.size(), CV_32F) };
    split(complexI, planesm);
    for (int h = 0; h < complexI.total(); h++) {
        planesm[0].at<float>(h) = planesm[0].at<float>(h)*this->filters[i].at<float>(h);
        planesm[1].at<float>(h) = planesm[1].at<float>(h)*this->filters[i].at<float>(h);
    }
    merge(planesm, 2, complexI);

    // Compute inverse DFT of the product
    Mat fi = Mat(I.size(), CV_32FC2);
    idft(complexI, fi);
    Mat planes2[] = { Mat::zeros(I.size(), CV_32F), Mat::zeros(I.size(), CV_32F) };
    split(fi, planes2);

    // Compute magnitude of the iDFT matrix
    Mat dft_I;
    magnitude(planes2[0], planes2[1], dft_I);
    dft_I += 1;
    log(dft_I, dft_I);
    normalize(dft_I, dft_I, 0, 1, CV_MINMAX);
    
//    if (i == 3)
//        imshow("Filtered", dft_I);
//    waitKey();
    return dft_I;
}

// Returns th features of some keypoints in the image. The keypoints are chosen uniformly with probability p
vector<array<float, FEAT_SIZE>> GALIF::features(const cv::Mat &I, double p) {
    // Compute filtered image
    this->compute_filters(I);
    Mat* R = new Mat[this->k];
    for (int i = 0; i < this->k; i++) {
        R[i] = this->filter(i, I);
    }
    
    // Compute 32 x 32 = 1024 keypoints evenly distributed
    int const DIV = 32;
    int const x_step = I.cols/(2*DIV);
    int const y_step = I.rows/(2*DIV);
    vector<Point> keypoints;
    for (int j = 0; j < DIV; j++) {
        for (int l = 0; l < DIV; l++) {
            keypoints.push_back(Point((2*j+1)*x_step, (2*l+1)*y_step));
        }
    }
    
    double const area_ratio = 0.075;
    double const img_area = double(I.rows * I.cols);
    double const bl_area = area_ratio * img_area;
    int bl_length = int(floor(sqrt(bl_area))); // côté d'un bloc servant au calcul de la feature d'un point-clé
    bl_length = (bl_length % 2 == 0) ? bl_length - 1 : bl_length;
    
    vector<array<float, FEAT_SIZE>> feats;
    int n = this->n;
    int bl_step, c_step;

    // Foreach keypoint, we divide the area around into n x n cells
    for (vector<Point>::iterator it = keypoints.begin(); it != keypoints.end(); ++it) {
        array<float, FEAT_SIZE> feat = {};
        // Iteration on the Gabor filter orientations
        float norm = 0.;
        bl_step = min(min(min(min((bl_length - 1)/2, it->x - 1), R[0].cols - it->x - 1), it->y-1), R[0].rows - it->y - 1);
        c_step = bl_step / n;
        for (int i = 0; i < this->k; i++) {
            Mat block = R[i](Rect(it->x - bl_step, it->y - bl_step, 2*bl_step, 2*bl_step));
            
            // Foreach cell, compute the local feature value
            for (int j = 0; j < n; j++) {
                for (int l = 0; l < n; l++) {
                    Mat cell = block(Rect(j*c_step, l*c_step, c_step, c_step));
                    feat[i*n*n + j*n + l] = sum(cell)(0);
                    norm += feat[i*n*n + j*n + l] * feat[i*n*n + j*n + l];
                }
            }
        }
        
        // Normalize the feature
        norm = sqrt(norm);
        if (norm > 0) {
            for (int i = 0; i < this->k * this->n * this->n; i++) {
                feat[i] /= norm;
            }
            
            // Add the feature to result
            feats.push_back(feat);
        }
    }
    
    // We keep only a certain amount of features in order to have 1024*p features by view at the end
    unsigned seed = unsigned(std::chrono::system_clock::now().time_since_epoch().count());
    default_random_engine gen(seed);
    uniform_real_distribution<double> unif_distr(0.0, 1.0);
    double tirage;
    double proba = p * (DIV * DIV) / max(int(feats.size()), 1);
    vector<array<float, FEAT_SIZE>> kept_feats;
    for (int i = 0; i < feats.size(); i++) {
        tirage = unif_distr(gen);
        if (tirage <= proba) {
            kept_feats.push_back(feats[i]);
        }
    }
    
    // Free memory
    delete[] R;
    
    cout << "Nombre de features : " << int(kept_feats.size()) << endl;
    return kept_feats;
}


