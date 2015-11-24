//
//  Point3.cpp
//  inf555
//
//  Created by Camille MASSET on 18/11/2015.
//  Copyright © 2015 Basile Camille. All rights reserved.
//

#include "Point3.hpp"
#include <math.h>

Point3::Point3() {
    this->x = 0.;
    this->y = 0.;
    this->z = 0.;
}

Point3::Point3(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

double Point3::distanceFrom(const Point3& b) const {
    return sqrt(this->x * b.x + this->y * b.y + this->z * b.z);
}

double Point3::distance2From(const Point3& b) const {
    return this->x * b.x + this->y * b.y + this->z * b.z;
}

Point3 Point3::operator+(const Point3 &b) const {
    return Point3(this->x + b.x, this->y + b.y, this->z + b.z);
}

Point3 Point3::operator*(double a) const {
    return Point3(this->x * a, this->y * a, this->z * a);
}