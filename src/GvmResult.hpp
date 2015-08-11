//
//  GvmResult.hpp
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
  class GvmResult {
  public:
    
    // The number of points in the cluster.
    
    int count;

    // The aggregate mass of the cluster.
    
    FP mass;
    
    // Defines the points that will be clusters
    
    S space;

    // The coordinates of the cluster's centroid.
    
    V point;

    // The variance of the cluster.

    FP variance;
    
    // The standard deviation of the cluster.
    
    FP stdDeviation;
    
    // The key associated with the cluster.
    
    K* key;

    // constructor
    
    GvmResult(GvmCluster<S,V,K,FP> cluster)
    : space(cluster.clusters.space)
    {
      count = cluster.count;
      mass = cluster.m0;
      variance = cluster.var / mass;
      stdDeviation = FP(-1.0);
      key = cluster.key;
      //space = cluster.clusters.space;
      point = space.newCopy(cluster.m1);
      space.scale(point, FP(1.0) / mass);
    }
    
    // getters
    
    // The number of points in the cluster.
    
    int getCount() {
      return count;
    }
    
    void setCount(int inCount) {
      count = inCount;
    }
    
    // The aggregate mass of the cluster.
    
    FP getMass() {
      return mass;
    }
    
    // Sets the aggregate mass of the cluster.
    
    void setMass(FP inMass) {
      mass = inMass;
    }
    
    S* getSpace() {
      return space;
    }
    
    void setSpace(S* inSpace) {
      space = inSpace;
    }
    
    // The coordinates of the cluster's centroid. The returned array should not
    // be modified.
    
    V getPoint() {
      return point;
    }
    
    // Sets the coordinates of the cluster's centroid. The values of the
    // supplied point are copied.
    
    void setPoint(V &inPoint) {
      point = inPoint;
    }

    // The variance of the cluster.
    
    FP getVariance() {
      return variance;
    }
    
    // The standard deviation of the cluster.
    
    FP getStdDeviation() {
      return stdDeviation < FP(0.0) ? stdDeviation = sqrt(variance) : stdDeviation;
    }
    
    // Sets the variance of the cluster.
    
    void setVariance(FP inVariance) {
      if (inVariance < FP(0.0)) {
        assert(0); // negative variance
      }
      variance = inVariance;
      stdDeviation = FP(-1.0);
    }
    
    // The key associated with the cluster, may be nullptr.
    
    K* getKey() {
      return key;
    }
    
    // Sets the key associated with the cluster.
    
    void setKey(K *inKey) {
      key = inKey;
    }
    
    // object methods
    
    std::string toString() {
      std::stringstream sb;
      char buffer[100];
      
      sb << space.toString(point);
      
      // FIXME: use generic toString() method defined on the specific key type
      
      snprintf(buffer, 100, "  count: %d  variance: %3.3f  mass: %3.3f  key: %p", count, variance, mass, key);
      sb << buffer;
            
      return sb.str();
    }
    
  }; // end class GvmResult

}
