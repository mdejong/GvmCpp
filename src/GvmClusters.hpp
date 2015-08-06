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

#import "GvmDefaultKeyer.hpp"

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
    
    // statics
    
    // Helper method to avoid propagation of negative variances.
    //
    // var : a variance
    // return the variance clamped at zero
    
    static inline double correct(double var) {
      return var >= 0.0 ? var : 0.0;
    }
    
    // The greatest number of clusters that will be recorded
    
    int capacity;
    
    // Defines the points that will be clusters
    
    S space;
    
    // The default keyer is implicitly defined and is
    // used unless setKeyer() is invoked to make
    // this cluster use a custom keyer.
    
    std::unique_ptr<GvmDefaultKeyer<S,K,P> > defaultKeyerPtr;
    
    // If a specific keyer is defined instead of the
    // default keyer then this pointer will be non=NULL
    
    GvmKeyer<S,K,P> *keyerPtr;
    
    // The clusters objects.

    std::vector<GvmCluster<S,K,P>* > clusters;
    
    // All possible cluster pairs.
    
    GvmClusterPairs<S,K,P> pairs;
    
    // The number of points that have been added.
    
    int additions;
    
    // The current number of clusters.
    
    int count;
    
    // The number of clusters prior to reduction
    
    int bound;
    
    // constructor
    
    GvmClusters<S,K,P>(S inSpace, int inCapacity)
    :
    space(inSpace),
    capacity(inCapacity),
    defaultKeyerPtr(new GvmDefaultKeyer<S,K,P>()),
    keyerPtr(NULL),
    pairs(capacity * (capacity-1) / 2),
    additions(0),
    count(0),
    bound(0)
    {
      assert(inCapacity > 0);
      
      clusters = std::vector<GvmCluster<S,K,P>* >(capacity);
      for (int i=0; i < capacity; i++) {
        clusters.push_back(NULL);
      }
    }
    
    // The keyer used to assign keys to clusters.
    
    GvmKeyer<S,K,P>* getKeyer() {
      if (keyerPtr != NULL) {
        return keyerPtr;
      } else {
        return defaultKeyerPtr;
      }
    }
    
    // Setter for keyer property, use this method to define a new keyer instead of
    // using GvmDefaultKeyer. Note that this object will not manage lifetime
    // of the inKeyer object. If NULL is passed into this method then the
    // default keyer will be used again.
    
    void setKeyer(GvmKeyer<S,K,P> *inKeyer) {
      keyerPtr = inKeyer;
    }
    
    int getCapacity() {
      return capacity;
    }
    
    S getSpace() {
      return space;
    }
    
    // Removes all clusters and clustered points but retains the keyer.
    
    void clear() {
      clusters.clear();
      pairs.clear();
      additions = 0;
      count = 0;
      bound = 0;
    }
    
    // FIXME: need impl of add and following methods
    
  }; // end class GvmClusters

}
