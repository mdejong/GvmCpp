//
//  GvmClusterPairs.hpp
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

// Maintains a heap of cluster pairs.

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
  class GvmClusterPairs {
  public:

    // Pairs is an array of pointers to GvmClusterPair objects
    
    std::vector<GvmClusterPair<S,K,P> > pairs;
    
    int size;
        
    GvmClusterPairs<S,K,P>(int initialCapacity)
    : size(0)
    {
      pairs.reserve(initialCapacity);
      return;
    }
    
    // Copy constructor
    
    GvmClusterPairs<S,K,P>(GvmClusterPairs<S,K,P> &that) {
      pairs = that.pairs;
      size = pairs.length;
    }
    
    GvmClusterPair<S,K,P>* peek() {
      return size == 0 ? nullptr : &pairs[0];
    }
    
    
    
    bool add(GvmClusterPair<S,K,P> &e) {
      /*
      //if (e == null) throw new IllegalArgumentException("null pair");
      int i = size;
      if (i >= pairs.length) grow(i + 1);
      size = i + 1;
      if (i == 0) {
        pairs[0] = e;
        e.index = 0;
      } else {
        heapifyUp(i, e);
      }
      */
      return true;
    }
    
    // add cluster pair and return ref to added pair object
    
    GvmClusterPair<S,K,P>& add(GvmCluster<S,K,P> &c1, GvmCluster<S,K,P> &c2) {
      pairs.push_back(GvmClusterPair<S,K,P>(c1, c2));
      return pairs[pairs.size() - 1];
    }

  }; // end class GvmClusterPairs

}
