//
//  Point3.hpp
//  inf555
//
//  Created by Camille MASSET on 18/11/2015.
//  Copyright © 2015 Basile Camille. All rights reserved.
//

#ifndef Point3_hpp
#define Point3_hpp

#include <stdio.h>

class Point3 {
public:
    double x;
    double y;
    double z;
    
    Point3(); // point (0, 0, 0)
    Point3(double x, double y, double z);
    double distanceFrom(const Point3& b) const;
    double distance2From(const Point3& b) const;
    Point3 operator+(Point3 const &b) const;
    Point3 operator*(double a) const;
};

#endif /* Point3_hpp */
