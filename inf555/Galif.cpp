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

float GALIF::gaussian(int i, double u, double v) const {
    assert(0 <= i && i < this->k);
    
    double theta = i * M_PI / this->k;
    
    double ut = u * cos(theta) - v * sin(theta);
    double vt = u * sin(theta) + v * cos(theta);
    
    return float(exp(-2 * M_PI_2 * ((ut - this->w0)*(ut - this->w0) * this->sigx*this->sigx + vt*vt * this->sigy*this->sigy)));
}

Mat GALIF::get_filter(int i, int rows, int cols) const {
    assert(rows > 0 && cols > 0);
    
    Mat fi = Mat(rows, cols, CV_32FC2);
    for (int l = 0; l < cols; l++) {
        for (int j = 0; j < rows; j++) {
            fi.at<float>(Point(l, j)) = this->gaussian(i, l, j);
        }
    }
    
    return fi;
}

void GALIF::compute_filters(Mat const &I) {
    int rows = getOptimalDFTSize(I.rows);
    int cols = getOptimalDFTSize(I.cols);
    
    for (int i = 0; i < this->k; i++) {
        this->filters[i] = this->get_filter(i, rows, cols);
    }
}

Mat GALIF::filter(int i, cv::Mat const &I) const {
    assert(I.type() == CV_32FC1);
    int m = I.rows;
    int p = I.cols;
    int dft_m = getOptimalDFTSize(I.rows);
    int dft_p = getOptimalDFTSize(I.cols);
    
    // Extand the image matrix
    Mat padded;
    copyMakeBorder(I, padded, 0, dft_m - m, 0, dft_p - p, BORDER_CONSTANT, Scalar::all(0));
    
    // Paste the extanded image in a complex matrix
    Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
    Mat complexI;
    merge(planes, 2, complexI);
    // Compute its DFT
    dft(complexI, complexI);
    // Compute point-wise multiplication by the gaussian filter
    complexI = complexI.mul(this->filters[i]);
    
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
    
//    imshow("Filtered", dft_I);
//    waitKey();
    
    return dft_I;
}

// Returns th features of some keypoints in the image. The keypoints are chosen uniformly with probability p
vector<float*> GALIF::features(const cv::Mat &I, double p) {
    this->compute_filters(I);
    
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine gen(seed);
    uniform_real_distribution<double> unif_distr(0.0, 1.0);
    
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
    
    vector<float*> feats;
    int count_kp = 0;
    double tirage;
    int n = this->n;
    int bl_step, c_step;
    
    Mat* R = new Mat[this->k];
    for (int i = 0; i < this->k; i++) {
        R[i] = this->filter(i, I);
    }
    
    // Foreach keypoint, we determine whether we keep it or not; if yes, we divide the area around into n x n cells
    for (vector<Point>::iterator it = keypoints.begin(); it != keypoints.end(); ++it) {
        tirage = unif_distr(gen);
        if (tirage > p) {
            continue;
        }
        count_kp += 1;
        
        float* feat = new float[this->k * this->n * this->n];
        
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
            for (int i = 0; i < this->k*this->n*this->n; i++) {
                feat[i] /= norm;
            }
            
            // Add the feature to result
            feats.push_back(feat);
        } else {
            count_kp -= 1;
        }
     }
    
    // Free memory
    delete[] R;
    
    cout << "Nombre de points-cles retenus : " << count_kp << endl;
    return feats;
}


