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
    
    // A cluster contains N keys which are typically
    // plain values inside a vector. But, the keys could
    // be any templated type that supports collecting
    // values in some user defined way. The key class must
    // support a default constructor and a copy constructor.
    
    K* keyPtr;
    
    // When a cluster is set to a collection of keys then
    // this vector will be populated with a copy of the
    // values of the keys collection. Note that keyPtr
    // will be set to the address of keyVec when values
    // have been inserted into keyVec.
    
    K keyVec;
    
    // constructor
    
    GvmCluster<S,V,K,FP>(GvmClusters<S,V,K,FP> &inClusters)
    : clusters(inClusters), removed(false), m0(0.0), var(0.0), keyPtr(nullptr), keyVec()
    {
      removed = false;
      count = 0;
      m0 = FP(0.0);
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
      return keyPtr;
    }
    
    // When a key is added to a vector or keys are merged then
    // this method is invoked.
    
    void setKey(K *aKey) {
      if (aKey == nullptr) {
        // Release any held ids at this point
        keyPtr = nullptr;
        // Invoke default constructor
        keyVec = K();
      } else if (keyPtr == aKey) {
        // Passed the current key vector pointer, which
        // is a nop since a combine operation would have
        // already appended new keys to the vector.
      } else {
#if defined(DEBUG)
        assert(keyVec.size() == 0);
#endif // DEBUG
        // Invoke copy constructor to fill key vec
        keyVec = K(*aKey);
        keyPtr = &keyVec;
      }
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
      setKey(nullptr);
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
