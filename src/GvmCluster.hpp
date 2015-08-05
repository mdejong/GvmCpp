//
//  GvmCluster.hpp
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
  class GvmCluster {
  public:
    
    // The set of clusters to which this cluster belongs
    
    GvmCluster<S,K,P> clusters;
    
    // The pairings of this cluster with all other clusters.
    
    //std::vector<GvmClusterPair<S,K,P>> pairs;
    
    // Whether this cluster is in the process of being removed.
    
    bool removed;
    
    // The number of points in this cluster.
    
    int count;
    
    // The total mass of this cluster.
    
    double m0;
    
    // The mass-weighted coordinate sum.
    
    std::vector<P> m1;

    // The mass-weighted coordinate-square sum.

    std::vector<P> m2;

    // The computed variance of this cluster.
    
    double var;

    // The key associated with this cluster.
    
    K key;
    
    // getters
    
    // The total mass of the cluster.
    
    double getMass() {
      return m0;
    }
    
    // The number of points in the cluster.
    
    int getCount() {
      return count;
    }
    
    // The computed variance of the cluster
    
    double getVariance() {
      return var;
    }
    
    // The key associated with the cluster, may be null.
    
    K getKey() {
      return key;
    }
    
    // constructor
    
    GvmCluster<S,K,P>(GvmCluster<S,K,P> &clusters)
    {
      this->clusters = clusters;
      removed = false;
      count = 0;
      m0 = 0.0;
      m1 = clusters.space.newOrigin();
      m2 = clusters.space.newOrigin();
//      pairs = vector<GvmClusterPair<S,K,P>>(clusters.size());
//      new GvmClusterPair[clusters.capacity];
//      update();
    }

    // package methods
    
    // Completely clears this cluster. All points and their associated mass is
    // removed along with any key that was assigned to the cluster,
    
    void clear();
    
    // Sets this cluster equal to a single point.
    
    void set(const double m, P pt);
    
//    private static double[] coords(Object obj) {
//      return (double[]) obj;
//    }

    
  }; // end class GvmCluster

}