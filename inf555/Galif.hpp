//
//  Galif.hpp
//  inf555
//
//  Created by Camille MASSET on 24/11/2015.
//  Copyright © 2015 Basile Camille. All rights reserved.
//

#ifndef Galif_hpp
#define Galif_hpp

#include <stdio.h>
#include <math.h>
#include <vector>
#include <array>

#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

class GALIF {
public:
    double w0;
    double sigx;
    double sigy; // computed in constructor
    int k; // number of filter orientations
    int n; // number of tiles
    cv::Mat* filters;
    static const int FEAT_SIZE = 256;
    
    GALIF(double w0, double sigx, int k, int n);
    ~GALIF();
    
    std::vector<std::array<float, FEAT_SIZE>> features(cv::Mat const &I, double p); // p is the probability for a keypoint to have its feature computed
    
//private:
    float gaussian(int i, double u, double v, int cols, int rows) const;
    cv::Mat get_filter(int i, int m, int p) const;
    void compute_filters(cv::Mat const &I);
    cv::Mat filter(int i, cv::Mat const &I) const;
};

#endif /* Galif_hpp */
