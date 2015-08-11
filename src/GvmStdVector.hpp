//
//  GvmStdVector.hpp
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
// This implementation of the "vector" interface is a simplified
// version that works like a std::vector<double> would except that
// the values are in a buffer that is a compile time fixed size.
// This makes the object a compile time fixed size so that an
// array of them can be allocated all at once and it makes access
// to a specific values faster since the operator[] overhead is less
// then it would be with a vector.

#import "GvmCommon.hpp"

namespace Gvm {
  
  // P
  //
  // Floating point type. This is the type (float or double)
  // that is used to represent the result of operations
  // in the vector space. Note that the value type V
  // can be smaller than this floating point range.
  
  // D
  //
  // Number of dimensions, typically 2 or 3.
  
  template<typename FP, int D>
  class GvmStdVector {
  public:
    
    // Statically defined array of values
    
    FP values[D];
    
    // Number of dimensions
    
    int getDimensions() {
      return D;
    }
    
    // constructor
    
    GvmStdVector<FP,D>()
    {
      assert(D >= 1);
      for (int i = 0; i < D; i++) {
        values[i] = FP(0.0);
      }
    }
    
    // copy constructor
    
    GvmStdVector<FP,D>(const GvmStdVector<FP,D> &other)
    {
      for (int i = 0; i < D; i++) {
        values[i] = other.values[i];
      }
    }
    
    /*
    
    // copy assignment
    
    GvmStdVector<FP,D>& operator=(const GvmStdVector<FP,D>& other)
    {
      for (int i = 0; i < D; i++) {
        values[i] = other.values[i];
      }
      return *this;
    }
    
    // move assignment
    
    GvmStdVector<FP,D>& operator=(const GvmStdVector<FP,D>&& other)
    {
      for (int i = 0; i < D; i++) {
        values[i] = other.values[i];
      }
      return *this;
    }
     
    */
    
    FP& operator[](std::size_t idx)       {
      return values[idx];
    };
    const FP& operator[](std::size_t idx) const {
      return values[idx];
    };
    
    FP iSquared(std::size_t idx) {
      FP v = values[idx];
      FP vSq = v * v;
      return vSq;
    }

    std::string toString() {
      std::stringstream sb;
      
      for (int i = 0; i < D; i++) {
        if (i < (D-1)) {
          sb << values[i] << " ";
        } else {
          sb << values[i];
        }
      }
      
      return sb.str();
    }
    
  }; // end class GvmStdVector

}