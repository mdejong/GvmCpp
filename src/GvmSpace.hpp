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

#import <vector>

using namespace std;

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
    
    virtual std::vector<P> newOrigin();
    
    virtual std::vector<P> newCopy(P pt);
    
    virtual double magnitudeSqr(P pt);
    
    virtual double sum(P pt);
    
  }; // end class GvmSpace

}