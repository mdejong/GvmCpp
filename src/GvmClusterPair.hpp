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
  //
  // Cluster vector space.
  
  // V
  //
  // Cluster vector type.
  
  // K
  //
  // Type of key.
  
  // FP
  //
  // Floating point type.
  
  template<typename S, typename V, typename K, typename FP>
  class GvmClusterPair {
  public:
    
    // The first cluster in this collection.
    
    GvmCluster<S,V,K,FP> &c1;
    
    // The second cluster in this collection.
    
    GvmCluster<S,V,K,FP> &c2;

    // The index of this pair within a heap of pairs.
    
    int index;

    // The amount the global variance would increase if this pair was merged.
    
    FP value;
    
    // Constructs a new pair and computes its value.
    //
    // @param c1 a cluster, not equal to c2
    // @param c2 a cluster, not equal to c1
    
    GvmClusterPair<S,V,K,FP>(GvmCluster<S,V,K,FP> &inC1, GvmCluster<S,V,K,FP> &inC2)
    : c1(inC1), c2(inC2), index(0)
    {
      if (&inC1 == &inC2) {
        assert(0);
      }
      this->update();
    }
    
    // Updates the value of the pair.
    
    void update() {
      value = c1.test(c2) - c1.var - c2.var;
    }

  }; // end class GvmClusterPair

}
