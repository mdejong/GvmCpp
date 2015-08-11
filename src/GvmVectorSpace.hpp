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

#import <tgmath.h>

namespace Gvm {
  
  // FP
  //
  // Floating point type that store the results of calculations
  // based on points. This floating point type could be float
  // or double.
  
  // V
  //
  // Vector of points type, a point is typically 2 or 3
  // values that represent (X,Y) or (X,Y,Z) coordinates
  // in 2D or 3D space. The type passed as V should
  // extend the GvmVector interface.
  
  template<typename V, typename FP, int D>
  class GvmVectorSpace {
  public:
    
    // The computed variance of the cluster
    
    int getDimensions() {
      return D;
    }
    
    // constructor
    
    GvmVectorSpace<FP,D>()
    {
      assert(D >= 1);
    }
    
    // space factory methods
    
    V newOrigin() {
      // Invoke default constructor
      return V();
    }
    
    V newCopy(V &pt) {
      // Invoke copy constructor
      return V(pt);
    }
    
    // space point operations
    
    FP magnitudeSqr(V &pt) {
      FP sum = FP(0.0);
      for (int i = 0; i < D; i++) {
        // sum += (pt[i] * pt[i]);
        sum += pt.iSquared(i);
      }
      return sum;
    }

    FP sum(V &pt) {
      FP sum = FP(0.0);
      for (int i = 0; i < D; i++) {
        FP c = pt[i];
        sum += c;
      }
      return sum;
    }
    
    void setToOrigin(V &pt) {
      for (int i = 0; i < D; i++) {
        pt[i] = FP(0.0);
      }
    }

    void setTo(V &dstPt, V &srcPt) {
      for (int i = 0; i < D; i++) {
        dstPt[i] = srcPt[i];
      }
    }

    void setToScaled(V &dstPt, FP m, V &srcPt) {
      for (int i = 0; i < D; i++) {
        dstPt[i] = m * srcPt[i];
      }
    }

    void setToScaledSqr(V &dstPt, FP m, V &srcPt) {
      for (int i = 0; i < D; i++) {
        // dstPt[i] = (srcPt[i] * srcPt[i]);
        dstPt[i] = m * srcPt.iSquared(i);
      }
    }

    void add(V &dstPt, V &srcPt) {
      for (int i = 0; i < D; i++) {
        dstPt[i] += srcPt[i];
      }
    }

    void addScaled(V &dstPt, FP m, V &srcPt) {
      for (int i = 0; i < D; i++) {
        dstPt[i] += m * srcPt[i];
      }
    }

    void addScaledSqr(V &dstPt, FP m, V &srcPt) {
      for (int i = 0; i < D; i++) {
        // dstPt[i] += m * (srcPt[i] * srcPt[i]);
        dstPt[i] += m * srcPt.iSquared(i);
      }
    }
    
    void subtract(V &dstPt, V &srcPt) {
      for (int i = 0; i < D; i++) {
        dstPt[i] -= srcPt[i];
      }
    }

    void subtractScaled(V &dstPt, FP m, V &srcPt) {
      for (int i = 0; i < D; i++) {
        dstPt[i] -= m * srcPt[i];
      }
    }
    
    void subtractScaledSqr(V &dstPt, FP m, V &srcPt) {
      for (int i = 0; i < D; i++) {
        // dstPt[i] -= m * (srcPt[i] * srcPt[i]);
        dstPt[i] -= m * srcPt.iSquared(i);
      }
    }
    
    void scale(V &pt, FP m) {
      for (int i = 0; i < D; i++) {
        pt[i] *= m;
      }
    }

    void square(V &pt) {
      for (int i = 0; i < D; i++) {
        pt[i] *= pt[i];
      }
    }

    // Note that this distance method is not used in the implementation
    // since it is much more effective to use delta squared directly.
    
    FP distance(V &pt1, V &pt2) {
      FP sum = FP(0.0);
      for (int i = 0; i < D; i++) {
        FP d = pt1[i] - pt2[i];
        sum += d * d;
      }
      return sqrt(sum);
    }
    
    // optimizations

    // FIXME: make optimized versions of functions below
    
    FP variance(FP m, V &pt, V &ptSqr) {
      FP sum = FP(0.0);
      const FP mMult = FP(1.0) / m;
      for (int i = 0; i < D; i++) {
        FP c = pt[i];
        sum += ptSqr[i] - ((c * c) * mMult);
      }
      return sum;
    }

    FP variance(FP m1, V &pt1, V &ptSqr1, FP m2, V &pt2) {
      const FP m0 = m1 + m2;
      const FP m0Mult = FP(1.0) / m0;
      FP sum = FP(0.0);
      for (int i = 0; i < D; i++) {
        FP c2 = pt2[i];
        FP c = pt1[i] + (m2 * c2);
        FP cSqr = ptSqr1[i] + (m2 * (c2 * c2));
        sum += cSqr - ((c * c) * m0Mult);
      }
      return sum;
    }

    FP variance(FP m1, V &pt1, V &ptSqr1, FP m2, V &pt2, V &ptSqr2) {
      const FP m0 = m1 + m2;
      const FP m0Mult = FP(1.0) / m0;
      FP sum = FP(0.0);
      for (int i = 0; i < D; i++) {
        FP c = pt1[i] + pt2[i];
        FP cSqr = ptSqr1[i] + ptSqr2[i];
        sum += cSqr - ((c * c) * m0Mult);
      }
      return sum;
    }

    std::string toString(V &pt) {
      return pt.toString();
    }
    
  }; // end class GvmVectorSpace

}