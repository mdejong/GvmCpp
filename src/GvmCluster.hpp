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
  class GvmCluster {
  public:
    
    // The set of clusters to which this cluster belongs
    
    GvmClusters<S,V,K,FP> &clusters;
    
    // The pairings of this cluster with all other clusters.
    // Note that this is a vector of pointers to cluster pairs.
    
    std::vector<std::shared_ptr<GvmClusterPair<S,V,K,FP>> > pairs;
    
    // Whether this cluster is in the process of being removed.
    
    bool removed;
    
    // The number of points in this cluster.
    
    int count;
    
    // The total mass of this cluster.
    
    FP m0;
    
    // The mass-weighted coordinate sum.
    
    V m1;

    // The mass-weighted coordinate-square sum.

    V m2;

    // The computed variance of this cluster.
    
    FP var;

    // The key associated with this cluster (can be nullptr).
    // Note that this ref is not a unique_ptr
    // or a shared_ptr, the lifetime of this
    // pointer must be managed by the caller.
    
    K* key;
    
    // constructor
    
    GvmCluster<S,V,K,FP>(GvmClusters<S,V,K,FP> &inClusters)
    : clusters(inClusters), removed(false), m0(0.0), var(0.0), key(nullptr)
    {
      removed = false;
      count = 0;
      m0 = FP(0.0);
      m1 = clusters.space.newOrigin();
      m2 = clusters.space.newOrigin();
      
      // FIXME: how many pairs for each cluster ?
      // If 2048 clusters and each one has 2048
      // pairs as initial capacity, then that is
      // (2048 * 8) bytes for null pointers.
      // 16384 = 16 kB for 1 cluster
      // 4 megs ?
      
      pairs.reserve(clusters.capacity);
      for (int i=0; i < clusters.capacity; i++) {
        pairs.push_back(nullptr);
      }
      
      update();
    }
    
    // getters
    
    // The total mass of the cluster.
    
    FP getMass() {
      return m0;
    }
    
    // The number of points in the cluster.
    
    int getCount() {
      return count;
    }
    
    // The computed variance of the cluster
    
    FP getVariance() {
      return var;
    }
    
    // The key associated with the cluster, may be nullptr.
    
    K* getKey() {
      return key;
    }
    
    // package methods
    
    // Completely clears this cluster. All points and their associated mass is
    // removed along with any key that was assigned to the cluster,
    
    void clear() {
      count = 0;
      m0 = FP(0.0);
      clusters.space.setToOrigin(m1);
      clusters.space.setToOrigin(m2);
      var = FP(0.0);
      key = nullptr;
    }
    
    // Sets this cluster equal to a single point.
    // m : the mass of the point
    // pt : the coordinates of the point
    
    void set(FP m, V &pt) {
      if (m == FP(0.0)) {
        if (count != 0) {
          clusters.space.setToOrigin(m1);
          clusters.space.setToOrigin(m2);
        }
      } else {
        clusters.space.setToScaled(m1, m, pt);
        clusters.space.setToScaledSqr(m2, m, pt);
      }
      count = 1;
      m0 = m;
      var = FP(0.0);
    }

    // Adds a point to the cluster.
    //
    // m : the mass of the point
    // pt : the coordinates of the point
    
    void add(FP m, V &pt) {
      if (count == 0) {
        set(m, pt);
      } else {
        count += 1;
        
        if (m != FP(0.0)) {
          m0 += m;
          clusters.space.addScaled(m1, m, pt);
          clusters.space.addScaledSqr(m2, m, pt);
          update();
        }
      }
    }

    // Sets this cluster equal to the specified cluster
    //
    // cluster : a cluster, not this or null
    
    void set(GvmCluster<S,V,K,FP> &cluster) {
      if (&cluster == this) {
        assert(0);
      }
      
      m0 = cluster.m0;
      clusters.space.setTo(m1, cluster.m1);
      clusters.space.setTo(m2, cluster.m2);
      var = cluster.var;
    }
    
    // Adds the specified cluster to this cluster.
    //
    // cluster : the cluster to be added
    
    void add(GvmCluster<S,V,K,FP> &cluster) {
      if (&cluster == this) {
        assert(0);
      }
      if (cluster.count == 0) return; //nothing to do
      
      if (count == 0) {
        set(cluster);
      } else {
        count += cluster.count;
        //TODO accelerate add
        m0 += cluster.m0;
        clusters.space.add(m1, cluster.m1);
        clusters.space.add(m2, cluster.m2);
        update();
      }
    }
    
    // Computes this clusters variance if it were to have a new point added to it.
    //
    // m the mass of the point
    // pt the coordinates of the point
    // return the variance of this cluster inclusive of the point
    
    FP test(FP m, V &pt) {
      return m0 == FP(0.0) && m == FP(0.0) ? FP(0.0) : clusters.space.variance(m0, m1, m2, m, pt) - var;
    }
    
    // Computes the variance of a cluster that aggregated this cluster with the
    // supplied cluster.
    //
    // cluster
    // another cluster
    // return the combined variance
    
    //TODO: change for consistency with other test method : return increase in variance
    
    FP test(GvmCluster<S,V,K,FP> &cluster) {
      return m0 == FP(0.0) && cluster.m0 == FP(0.0) ? FP(0.0) : clusters.space.variance(m0, m1, m2, cluster.m0, cluster.m1, cluster.m2);
    }
    
    // Recompute this cluster's variance.
    
    void update() {
      var = m0 == FP(0.0) ? FP(0.0) : clusters.space.variance(m0, m1, m2);
    }
    
  }; // end class GvmCluster

}
