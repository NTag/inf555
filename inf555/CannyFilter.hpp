//
//  Canny.hpp
//  inf555
//
//  Created by Camille MASSET on 18/11/2015.
//  Copyright © 2015 Basile Camille. All rights reserved.
//

#ifndef CannyFilter_hpp
#define CannyFilter_hpp

#include <stdio.h>
#include "opencv2/imgproc.hpp"

class CannyFilter {
private:
    double low_threshold;
    double high_threshold;
    
public:
    CannyFilter(double low, double high);
    cv::Mat detectEdges(cv::Mat& img);
};

#endif /* CannyFilter_hpp */
