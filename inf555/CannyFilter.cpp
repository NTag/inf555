//
//  Canny.cpp
//  inf555
//
//  Created by Camille MASSET on 18/11/2015.
//  Copyright © 2015 Basile Camille. All rights reserved.
//

#include "CannyFilter.hpp"

#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

using namespace std;
using namespace cv;

CannyFilter::CannyFilter(double low, double high) {
    low_threshold = low;
    high_threshold = high;
}


Mat CannyFilter::detectEdges(Mat &img) {
    Mat gray, edges, out;
    out.create(img.size(), img.type());
    
    cvtColor(img, gray, COLOR_BGR2GRAY);
    
    blur(gray, edges, Size(3, 3));
    Canny(edges, edges, this->low_threshold, this->high_threshold, 3);
    
    out = Scalar::all(0.);
    img.copyTo(out, edges);
    
    return edges;
}