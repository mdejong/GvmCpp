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
    
    GvmClusters<S,K,P> &clusters;
    
    // The pairings of this cluster with all other clusters.
    // Note that this is a vector of pointers to cluster pairs.
    
    std::vector<std::shared_ptr<GvmClusterPair<S,K,P>> > pairs;
    
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

    // The key associated with this cluster (can be nullptr).
    // Note that this ref is not a unique_ptr
    // or a shared_ptr, the lifetime of this
    // pointer must be managed by the caller.
    
    K* key;
    
    // constructor
    
    GvmCluster<S,K,P>(GvmClusters<S,K,P> &inClusters)
    : clusters(inClusters), pairs(), removed(false), m0(0.0), var(0.0), key(nullptr)
    {
      removed = false;
      count = 0;
      m0 = 0.0;
      m1 = clusters.space.newOrigin();
      m2 = clusters.space.newOrigin();
      
      pairs.reserve(clusters.capacity);
      for (int i=0; i < clusters.capacity; i++) {
        pairs.push_back(nullptr);
      }
      
      update();
    }
    
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
    
    // The key associated with the cluster, may be nullptr.
    
    K* getKey() {
      return key;
    }
    
    // package methods
    
    // Completely clears this cluster. All points and their associated mass is
    // removed along with any key that was assigned to the cluster,
    
    void clear() {
      count = 0;
      m0 = 0.0;
      clusters.space.setToOrigin(m1);
      clusters.space.setToOrigin(m2);
      var = 0.0;
      key = nullptr;
    }
    
    // Sets this cluster equal to a single point.
    // m : the mass of the point
    // pt : the coordinates of the point
    
    void set(double m, std::vector<P> &pt) {
      if (m == 0.0) {
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
      var = 0.0;
    }

    // Adds a point to the cluster.
    //
    // m : the mass of the point
    // pt : the coordinates of the point
    
    void add(double m, std::vector<P> &pt) {
      if (count == 0) {
        set(m, pt);
      } else {
        count += 1;
        
        if (m != 0.0) {
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
    
    void set(GvmCluster<S,K,P> &cluster) {
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
    
    void add(GvmCluster<S,K,P> &cluster) {
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
    
    double test(double m, std::vector<P> &pt) {
      return m0 == 0.0 && m == 0.0 ? 0.0 : clusters.space.variance(m0, m1, m2, m, pt) - var;
    }
    
    // Computes the variance of a cluster that aggregated this cluster with the
    // supplied cluster.
    //
    // cluster
    // another cluster
    // return the combined variance
    
    //TODO: change for consistency with other test method : return increase in variance
    
    double test(GvmCluster<S,K,P> &cluster) {
      return m0 == 0.0 && cluster.m0 == 0.0 ? 0.0 : clusters.space.variance(m0, m1, m2, cluster.m0, cluster.m1, cluster.m2);
    }
    
    // Recompute this cluster's variance.
    
    void update() {
      var = m0 == 0.0 ? 0.0 : clusters.space.variance(m0, m1, m2);
    }
    
  }; // end class GvmCluster

}
