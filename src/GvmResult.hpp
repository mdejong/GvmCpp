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
  class GvmResult {
  public:
    
    // The number of points in the cluster.
    
    int count;

    // The aggregate mass of the cluster.
    
    double mass;
    
    // Defines the points that will be clusters
    
    S space;

    // The coordinates of the cluster's centroid.
    
    std::vector<P> point;

    // The variance of the cluster.

    double variance;
    
    // The standard deviation of the cluster.
    
    double stdDeviation;
    
    // The key associated with the cluster.
    
    K* key;

    // constructor
    
    GvmResult(GvmCluster<S,K,P> cluster)
    : space(cluster.clusters.space)
    {
      count = cluster.count;
      mass = cluster.m0;
      variance = cluster.var / mass;
      stdDeviation = -1.0;
      key = cluster.key;
      //space = cluster.clusters.space;
      point = space.newCopy(cluster.m1);
      space.scale(point, 1.0 / mass);
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
    
    double getMass() {
      return mass;
    }
    
    // Sets the aggregate mass of the cluster.
    
    void setMass(double inMass) {
      mass = inMass;
    }
    
    GvmSpace<P>* getSpace() {
      return space;
    }
    
    void setSpace(GvmSpace<P>* inSpace) {
      space = inSpace;
    }
    
    // The coordinates of the cluster's centroid. The returned array should not
    // be modified.
    
    std::vector<P> getPoint() {
      return point;
    }
    
    // Sets the coordinates of the cluster's centroid. The values of the
    // supplied point are copied.
    
    void setPoint(std::vector<P> &inPoint) {
      point = inPoint;
    }

    // The variance of the cluster.
    
    double getVariance() {
      return variance;
    }
    
    // The standard deviation of the cluster.
    
    double getStdDeviation() {
      return stdDeviation < 0.0 ? stdDeviation = sqrt(variance) : stdDeviation;
    }
    
    // Sets the variance of the cluster.
    
    void setVariance(double inVariance) {
      if (inVariance < 0.0) {
        assert(0); // negative variance
      }
      variance = inVariance;
      stdDeviation = -1.0;
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
      
      snprintf(buffer, 100, "  count: %d  variance: %3.3f  mass: %3.3f  key: %s", count, variance, mass, key);
      sb << buffer;
      
      return sb.str();
    }
    
  }; // end class GvmResult

}
