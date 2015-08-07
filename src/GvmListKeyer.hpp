//
//  GvmListKeyer.hpp
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

// Allows multiple keys to be associated with clusters in the form of a lists
// which may be concatenated when clusters merge. Note that a list is assumed
// to be implemented with a C++ vector of a specific type.

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
  class GvmListKeyer : public GvmSimpleKeyer<S,K,P> {
  public:
    
    GvmListKeyer<S,K,P>()
    {
    }
    
    // Called when two clusters are being merged. One key needs to be
    // chosen/synthesized from those of the clusters being merged.
    //
    // c1 : the cluster with the greater mass
    // c2 : the cluster with the lesser mass
    // return a key for the cluster that combines those of c1 and c2, may be null
    
    K* combineKeys(K* list1, K* list2)
    {
      for ( auto &p : *list2 ) {
        list1->push_back(p);
      }
      //list1->insert(list1->end(), list2->begin(), list2->end());
      return list1;
    }
    
  }; // end class GvmListKeyer

}
