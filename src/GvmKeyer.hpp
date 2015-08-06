//
//  GvmKeyer.hpp
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

// Controls how new keys are added to clusters and how keys are combined when clusters merge.
// Any number of strategies are possible including:
//
// some applications may simply discard all keys (if the salient information is simply the cluster sizes),
// some applications may choose the key of the largest cluster (if the salient information is an exemplar for each cluster),
// and some applications may choose to maintain a list of keys (if every key needs to be mapped back to a cluster).

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
  class GvmKeyer {
  public:
    
    // Called when two clusters are being merged. One key needs to be
    // chosen/synthesized from those of the clusters being merged.
    //
    // c1 : the cluster with the greater mass
    // c2 : the cluster with the lesser mass
    // return a key for the cluster that combines those of c1 and c2, may be null
    
    virtual K mergeKeys(GvmCluster<S,K,P> &c1, GvmCluster<S,K,P> &c2);
    
    // Called when a key is being added to a cluster.
    //
    // cluster
    // key : the key for a newly clustered coordinate
    // return the key to be assigned to the new cluster, may be null

    virtual K addKey(GvmCluster<S,K,P> &cluster, K key);
    
  }; // end class GvmKeyer

}
