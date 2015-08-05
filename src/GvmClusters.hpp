//
//  GvmClusters.hpp
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

// Maintains a collection of clusters which are adjusted as new elements are
// added. The keyer assigned to this object should not be modified while there
// are clusters.

#import "GvmCommon.hpp"

namespace Gvm {
  // S
  
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
  class GvmClusters {
  public:
    
    // The greatest number of clusters that will be recorded
    
    int capacity;
    
    // Defines the points that will be clusters
    
    S space;
    
    GvmClusters<S,K,P>(S inSpace, int inCapacity)
    : space(inSpace), capacity(inCapacity)
    {
      assert(inCapacity > 0);
      
      //this.clusters = new GvmCluster[capacity];
      //pairs = new GvmClusterPairs<S,K>(capacity * (capacity-1) / 2);
    }
    
  }; // end class GvmClusters

}