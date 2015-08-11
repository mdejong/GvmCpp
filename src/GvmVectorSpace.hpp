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
  
  template<typename P, int D>
  class GvmVectorSpace : public GvmSpace<P> {
  public:
    
    // The computed variance of the cluster
    
    int getDimensions() {
      return D;
    }
    
    // constructor
    
    GvmVectorSpace<P,D>()
    {
      assert(D >= 1);
    }
    
    // space factory methods
    
    std::vector<P> newOrigin() {
      return std::vector<P>(D);
    }
    
    std::vector<P> newCopy(std::vector<P> &pt) {
      return std::vector<P>(pt);
    }
    
    // space point operations
    
    P magnitudeSqr(std::vector<P> &pt) {
      P sum = P(0.0);
      for (int i = 0; i < D; i++) {
        P c = pt[i];
        sum += (c * c);
      }
      return sum;
    }

    P sum(std::vector<P> &pt) {
      P sum = P(0.0);
      for (int i = 0; i < D; i++) {
        P c = pt[i];
        sum += c;
      }
      return sum;
    }
    
    void setToOrigin(std::vector<P> &pt) {
      for (int i = 0; i < D; i++) {
        pt[i] = P(0.0);
      }
    }

    void setTo(std::vector<P> &dstPt, std::vector<P> &srcPt) {
      for (int i = 0; i < D; i++) {
        dstPt[i] = srcPt[i];
      }
    }

    void setToScaled(std::vector<P> &dstPt, P m, std::vector<P> &srcPt) {
      for (int i = 0; i < D; i++) {
        dstPt[i] = m * srcPt[i];
      }
    }

    void setToScaledSqr(std::vector<P> &dstPt, P m, std::vector<P> &srcPt) {
      for (int i = 0; i < D; i++) {
        P c = srcPt[i];
        dstPt[i] = m * (c * c);
      }
    }

    void add(std::vector<P> &dstPt, std::vector<P> &srcPt) {
      for (int i = 0; i < D; i++) {
        dstPt[i] += srcPt[i];
      }
    }

    void addScaled(std::vector<P> &dstPt, P m, std::vector<P> &srcPt) {
      for (int i = 0; i < D; i++) {
        dstPt[i] += m * srcPt[i];
      }
    }

    void addScaledSqr(std::vector<P> &dstPt, P m, std::vector<P> &srcPt) {
      for (int i = 0; i < D; i++) {
        P c = srcPt[i];
        dstPt[i] += m * (c * c);
      }
    }
    
    void subtract(std::vector<P> &dstPt, std::vector<P> &srcPt) {
      for (int i = 0; i < D; i++) {
        dstPt[i] -= srcPt[i];
      }
    }

    void subtractScaled(std::vector<P> &dstPt, P m, std::vector<P> &srcPt) {
      for (int i = 0; i < D; i++) {
        dstPt[i] -= m * srcPt[i];
      }
    }
    
    void subtractScaledSqr(std::vector<P> &dstPt, P m, std::vector<P> &srcPt) {
      for (int i = 0; i < D; i++) {
        P c = srcPt[i];
        dstPt[i] -= m * (c * c);
      }
    }
    
    void scale(std::vector<P> &pt, P m) {
      for (int i = 0; i < D; i++) {
        pt[i] *= m;
      }
    }

    void square(std::vector<P> &pt) {
      for (int i = 0; i < D; i++) {
        pt[i] *= pt[i];
      }
    }

    // optimizations

    P distance(std::vector<P> &pt1, std::vector<P> &pt2) {
      P sum = P(0.0);
      for (int i = 0; i < D; i++) {
        P d = pt1[i] - pt2[i];
        sum += d * d;
      }
      return sqrt(sum);
    }
    
    P variance(P m, std::vector<P> &pt, std::vector<P> &ptSqr) {
      P sum = P(0.0);
      const P mMult = P(1.0) / m;
      for (int i = 0; i < D; i++) {
        P c = pt[i];
        sum += ptSqr[i] - ((c * c) * mMult);
      }
      return sum;
    }

    P variance(P m1, std::vector<P> &pt1, std::vector<P> &ptSqr1, P m2, std::vector<P> &pt2) {
      const P m0 = m1 + m2;
      const P m0Mult = P(1.0) / m0;
      P sum = P(0.0);
      for (int i = 0; i < D; i++) {
        P c2 = pt2[i];
        P c = pt1[i] + (m2 * c2);
        P cSqr = ptSqr1[i] + (m2 * (c2 * c2));
        sum += cSqr - ((c * c) * m0Mult);
      }
      return sum;
    }

    P variance(P m1, std::vector<P> &pt1, std::vector<P> &ptSqr1, P m2, std::vector<P> &pt2, std::vector<P> &ptSqr2) {
      const P m0 = m1 + m2;
      const P m0Mult = P(1.0) / m0;
      P sum = P(0.0);
      for (int i = 0; i < D; i++) {
        P c = pt1[i] + pt2[i];
        P cSqr = ptSqr1[i] + ptSqr2[i];
        sum += cSqr - ((c * c) * m0Mult);
      }
      return sum;
    }

    std::string toString(std::vector<P> &pt) {
      std::stringstream sb;
      
      for (int i = 0; i < D; i++) {
        if (i < (D-1)) {
          sb << pt[i] << " ";
        } else {
          sb << pt[i];
        }
      }
      
      return sb.str();
    }
    
  }; // end class GvmVectorSpace

}