//
//  GvmClusterPair.hpp
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

  // S
  // Specific subclass that implements GvmSpace
  
  // K
  //
  // Type of key
  
  // P
  //
  // Point type. For example a 2D set of points could be
  // represented by a type that was large enough to support
  // 2 float or double numbers. There can be many instances
  // of a point and a copy operation should be a fast as
  // possible so this type should be space optimized so
  // that only the required amount of memory is needed
  // to represent a specific kind of point.
  
  template<typename S, typename K, typename P>
  class GvmClusterPair {
  public:
    
    // The first cluster in this collection.
    
    GvmCluster<S,K,P> &c1;
    
    // The second cluster in this collection.
    
    GvmCluster<S,K,P> &c2;

    // The index of this pair within a heap of pairs.
    
    int index;

    // The amount the global variance would increase if this pair was merged.
    
    double value;
    
    // Constructs a new pair and computes its value.
    //
    // @param c1 a cluster, not equal to c2
    // @param c2 a cluster, not equal to c1
    
    GvmClusterPair<S,K,P>(GvmCluster<S,K,P> &inC1, GvmCluster<S,K,P> &inC2)
    : c1(inC1), c2(inC2)
    {
      if (&inC1 == &inC2) {
        assert(0);
      }
      this->update();
    }
    
    void update() {
      value = c1.test(c2) - c1.var - c2.var;
    }

  }; // end class GvmClusterPair

}
