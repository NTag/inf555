//
//  DistributedViews.hpp
//  inf555
//
//  Created by Camille MASSET on 17/11/2015.
//  Copyright © 2015 Basile Camille. All rights reserved.
//

#ifndef DistributedViews_hpp
#define DistributedViews_hpp

#include <stdio.h>
#include <vector>
#include "Point3.hpp"

class DistributedViews {
public:
    DistributedViews(int d);
    int getNum() const;
    Point3* getDirections();
    std::pair<double, Point3>* getRotations(); // TODO
    
private:
    int d;
    double MSE;
    Point3* centroids;
    std::vector<Point3> vertices;
    
    void initSeeds();
};

#endif /* DistributedViews_hpp */
