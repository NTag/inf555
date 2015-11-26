//
//  Galif.cpp
//  inf555
//
//  Created by Camille MASSET on 24/11/2015.
//  Copyright © 2015 Basile Camille. All rights reserved.
//

#include "Galif.hpp"

#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"


using namespace std;
using namespace cv;


GALIF::GALIF(double w0, double sigx, int k, int n) {
    assert(k > 0);
    this->w0 = w0;
    this->sigx = sigx;
    this->k = k;
    this->n = n;
    
    this->sigy = 1./100;
}

float GALIF::gaussian(int i, double u, double v) const {
    assert(0 <= i && i < this->k);
    
    double theta = i * M_PI / this->k;
    
    double ut = u * cos(theta) - v * sin(theta);
    double vt = u * sin(theta) + v * cos(theta);
    
    return float(exp(-2 * M_PI_2 * ((ut - this->w0)*(ut - this->w0) * this->sigx*this->sigx + vt*vt * this->sigy*this->sigy)));
}

Mat GALIF::get_filter(int i, int m, int p) const {
    assert(m > 0 && p > 0);
    
    Mat fi = Mat(m, p, CV_32FC1);
    for (int k = 0; k < p; k++) {
        for (int j = 0; j < m; j++) {
            fi.at<float>(Point(k, j)) = this->gaussian(i, k, j);
        }
    }
    
    return fi;
}

Mat GALIF::filter(int i, cv::Mat const &I) const {
    assert(I.type() == CV_32FC1);
    int m = I.rows;
    int p = I.cols;
    int dft_m = getOptimalDFTSize(I.rows);
    int dft_p = getOptimalDFTSize(I.cols);
    
    Mat filter_mat = this->get_filter(i, dft_m, dft_p);
    Mat padded, dft_I;
    copyMakeBorder(I, padded, 0, dft_m - m, 0, dft_p - p, BORDER_CONSTANT, Scalar::all(0));
    
    Mat planes[] = {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};
    Mat complexI;
    merge(planes, 2, complexI);
    
    dft(complexI, complexI);
    split(complexI, planes);
    dft_I = dft_I.mul(filter_mat);
    
    Mat fi = Mat(m, p, CV_32FC1);
    idft(dft_I, fi);
    
    return fi;
}

Mat* GALIF::filter(Mat const &I) const {
    Mat* fis = new Mat[this->k];
    
    for (int i = 0; i < this->k; i++) {
        fis[i] = this->filter(i, I);
    }
    
    return fis;
}

Mat GALIF::non_normalized_feature(int i, const cv::Mat &I) const {
    assert(i >= 0 && i < this->k);
    
    int n = this->n;
    Mat feat = Mat(n, n, CV_32F);
    Mat cell;
    int x_s, y_s;
    
    for (int j = 0; j < n; j++) {
        for (int l = 0; l < n; l++) {
            x_s = n / k;
            y_s = n / k;
            cell = I(Rect(j, l, x_s, y_s));
            assert(sum(cell).isReal());
            feat.at<float>(Point(j, l)) = sum(cell)(0);
        }
    }
    
    return feat;
}

Mat* GALIF::feature(const cv::Mat &I) const {
    Mat* feats = new Mat[this->k];
    Mat feat;
    for (int i = 0; i < this->k; i++) {
        feat = this->non_normalized_feature(i, I);
        normalize(feat, feat);
        feats[i] = 1./this->k * feat;
    }
    
    return feats;
}


