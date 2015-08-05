//
//  GvmVectorSpace.hpp
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

#import <vector>
#import <sstream>

#import "GvmSpace.hpp"

#import <tgmath.h>

namespace Gvm {
  
  // P
  //
  // Point type. For example a 2D set of points could be
  // represented by a type that was large enough to support
  // 2 float or double numbers. There can be many instances
  // of a point and a copy operation should be a fast as
  // possible so this type should be space optimized so
  // that only the required amount of memory is needed
  // to represent a specific kind of point.
  
  template<typename P>
  class GvmVectorSpace {
  public:
    
    int dimensions;
    
    // The computed variance of the cluster
    
    int getDimensions() {
      return dimensions;
    }
    
    // constructor
    
    GvmVectorSpace<P>(int dimensions) {
      assert(dimensions >= 1);
      this->dimensions = dimensions;
    }
    
    // space factory methods
    
    std::vector<P> newOrigin() {
      return std::vector<P>(dimensions);
    }
    
    std::vector<P> newCopy(std::vector<P> &pt) {
      return std::vector<P>(pt);
    }
    
    // space point operations
    
    double magnitudeSqr(std::vector<P> &pt) {
      double sum = 0.0;
      for ( P c : pt ) {
        sum += (double) (c * c);
      }
      return sum;
    }

    double sum(std::vector<P> &pt) {
      double sum = 0.0;
      for ( P c : pt ) {
        sum += c;
      }
      return sum;
    }
    
    void setToOrigin(std::vector<P> &pt) {
      for (int i = 0; i < dimensions; i++) {
        pt[i] = 0.0;
      }
    }

    void setTo(std::vector<P> &dstPt, std::vector<P> &srcPt) {
      for (int i = 0; i < dimensions; i++) {
        dstPt[i] = srcPt[i];
      }
    }

    void setToScaled(std::vector<P> &dstPt, double m, std::vector<P> &srcPt) {
      for (int i = 0; i < dimensions; i++) {
        dstPt[i] = m * srcPt[i];
      }
    }

    void setToScaledSqr(std::vector<P> &dstPt, double m, std::vector<P> &srcPt) {
      for (int i = 0; i < dimensions; i++) {
        double c = srcPt[i];
        dstPt[i] = m * c * c;
      }
    }

    void add(std::vector<P> &dstPt, std::vector<P> &srcPt) {
      for (int i = 0; i < dimensions; i++) {
        dstPt[i] += srcPt[i];
      }
    }

    void addScaled(std::vector<P> &dstPt, double m, std::vector<P> &srcPt) {
      for (int i = 0; i < dimensions; i++) {
        dstPt[i] += m * srcPt[i];
      }
    }

    void addScaledSqr(std::vector<P> &dstPt, double m, std::vector<P> &srcPt) {
      for (int i = 0; i < dimensions; i++) {
        double c = srcPt[i];
        dstPt[i] = m * c * c;
      }
    }
    
    void subtract(std::vector<P> &dstPt, std::vector<P> &srcPt) {
      for (int i = 0; i < dimensions; i++) {
        dstPt[i] -= srcPt[i];
      }
    }

    void subtractScaled(std::vector<P> &dstPt, double m, std::vector<P> &srcPt) {
      for (int i = 0; i < dimensions; i++) {
        dstPt[i] -= m * srcPt[i];
      }
    }
    
    void subtractScaledSqr(std::vector<P> &dstPt, double m, std::vector<P> &srcPt) {
      for (int i = 0; i < dimensions; i++) {
        double c = srcPt[i];
        dstPt[i] -= m * c * c;
      }
    }
    
    void scale(std::vector<P> &pt, double m) {
      for (int i = 0; i < dimensions; i++) {
        pt[i] *= m;
      }
    }

    void square(std::vector<P> &pt) {
      for (int i = 0; i < dimensions; i++) {
        pt[i] *= pt[i];
      }
    }

    // optimizations

    double distance(std::vector<P> &pt1, std::vector<P> &pt2) {
      double sum = 0.0;
      for (int i = 0; i < dimensions; i++) {
        double d = pt1[i] - pt2[i];
        sum += d * d;
      }
      return sqrt(sum);
    }
    
    double variance(double m, std::vector<P> &pt, std::vector<P> &ptSqr) {
      double sum = 0.0;
      for (int i = 0; i < dimensions; i++) {
        double c = pt[i];
        sum += ptSqr[i] - ((c * c) / m);
      }
      return sum;
    }

    double variance(double m1, std::vector<P> &pt1, std::vector<P> &ptSqr1, double m2, std::vector<P> pt2) {
      double m0 = m1 + m2;
      double sum = 0.0;
      for (int i = 0; i < dimensions; i++) {
        double c2 = pt2[i];
        double c = pt1[i] + (m2 * c2);
        double cSqr = ptSqr1[i] + (m2 * c2 * c2);
        sum += cSqr - ((c * c) / m0);
      }
      return sum;
    }

    double variance(double m1, std::vector<P> &pt1, std::vector<P> &ptSqr1, double m2, std::vector<P> pt2, std::vector<P> &ptSqr2) {
      double m0 = m1 + m2;
      double sum = 0.0;
      for (int i = 0; i < dimensions; i++) {
        double c = pt1[i] + pt2[i];
        double cSqr = ptSqr1[i] + ptSqr2[i];
        sum += cSqr - ((c * c) / m0);
      }
      return sum;
    }

    string toString(std::vector<P> &pt) {
      std::stringstream sb;
      
      for (int i = 0; i < dimensions; i++) {
        if (i < (dimensions-1)) {
          sb << pt[i] << " ";
        } else {
          sb << pt[i];
        }
      }
      
      return sb.str();
    }
    
  }; // end class GvmVectorSpace

}