//
//  main.cpp
//  inf555
//
//  Created by Basile Bruneau on 04/11/2015.
//  Copyright © 2015 Basile Camille. All rights reserved.
//

#include "CannyFilter.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

using namespace std;
using namespace cv;

int main(int argc, char** argv) {
    Mat img = imread("../fruits.JPG");
    CannyFilter canny = CannyFilter(10, 30);
    Mat edges = canny.detectEdges(img);
    imshow("Edges", edges); waitKey(0);
    
    return 0;
}