//
//  main.cpp
//  inf555
//
//  Created by Basile Bruneau on 04/11/2015.
//  Copyright © 2015 Basile Camille. All rights reserved.
//

#include <iostream>

#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include "CannyFilter.hpp"
#include "Galif.hpp"

using namespace std;
using namespace cv;

/**
 * Usage : 
 * string ty =  type2str( img.type() );
 * printf("Matrix: %s %dx%d \n", ty.c_str(), img.cols, img.rows );
 */
string type2str(int type) {
    string r;
    
    uchar depth = type & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (type >> CV_CN_SHIFT);
    
    switch (depth) {
        case CV_8U:  r = "8U"; break;
        case CV_8S:  r = "8S"; break;
        case CV_16U: r = "16U"; break;
        case CV_16S: r = "16S"; break;
        case CV_32S: r = "32S"; break;
        case CV_32F: r = "32F"; break;
        case CV_64F: r = "64F"; break;
        default:     r = "User"; break;
    }
    
    r += "C";
    r += (chans+'0');
    
    return r;
}

int main(int argc, char** argv) {
    Mat img = imread("../fruits.JPG", 0);

    double minVal, maxVal;
    minMaxLoc(img, &minVal, &maxVal);
    img.convertTo(img, CV_32F, 1./(maxVal-minVal));
    imshow("IMG", img);waitKey();
    
    GALIF galif = GALIF(100, 1./50, 4, 8);
    
//    Mat gf = galif.get_filter(1, 600, 600);
//    imshow("IMG", gf);waitKey();
//    
//    Mat dft_img = Mat(getOptimalDFTSize(gf.rows), getOptimalDFTSize(gf.cols), CV_32FC2);
//    idft(gf, dft_img);
//    imshow("IMG", dft_img);waitKey();
//    
//    double minFreq, maxFreq;
//    Point minPt, maxPt;
//    minMaxLoc(dft_img, &minFreq, &maxFreq, &minPt, &maxPt);
//    cout << maxPt << endl;
    
    galif.feature(img);
    
    return 0;
}