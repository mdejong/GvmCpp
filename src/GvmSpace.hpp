//
//  GvmSpace.hpp
//  GvmCpp
//
//  Created by Mo DeJong on 8/14/15.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//

#import "GvmCommon.hpp"

namespace Gvm {

  // GvmSpace is an abstract class that defines how calculations of
  // distance and variance in the given 2D or 3D space are calculated.
  // The user would extend this class to provide a specific impl.
  
  template<typename P>
  class GvmSpace {
  public:
    
    //TODO use within cluster
    
    static inline double nonNegative(double v) {
      return v < 0.0 ? 0.0 : v;
    }
    
    virtual std::vector<P> newOrigin() = 0;
    
    virtual std::vector<P> newCopy(std::vector<P> &pt) = 0;
    
    virtual double magnitudeSqr(std::vector<P> &pt) = 0;
    
    virtual double sum(std::vector<P> &pt) = 0;
    
    //not used directly in algorithm, but useful - override for good performance
    
    double magnitude(std::vector<P> &pt)
    {
      return sqrt(magnitudeSqr(pt));
    }
    
    //not used directly in algorithm, but useful - override for good performance
    
    double distance(std::vector<P> &pt1, std::vector<P> &pt2)
    {
      std::vector<P> p = pt1.newCopy(pt1);
      subtract(p, pt2);
      return magnitude(p);
    }
    
    virtual double variance(double m, std::vector<P> &pt, std::vector<P> &ptSqr) = 0;
    
    virtual double variance(double m1, std::vector<P> &pt1, std::vector<P> &ptSqr1, double m2, std::vector<P> pt2) = 0;
    
    virtual double variance(double m1, std::vector<P> &pt1, std::vector<P> &ptSqr1, double m2, std::vector<P> pt2, std::vector<P> &ptSqr2) = 0;
    
  }; // end class GvmSpace

}