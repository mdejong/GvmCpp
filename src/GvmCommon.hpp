//
//  GvmCommon.hpp
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

#import <vector>

#import <sstream>

#import <assert.h>

namespace Gvm {
  template<typename S, typename V, typename K, typename FP> class GvmCluster;
  template<typename S, typename V, typename K, typename FP> class GvmClusters;
  
  template<typename S, typename V, typename K, typename FP> class GvmClusterPair;
  template<typename S, typename V, typename K, typename FP> class GvmClusterPairs;
  
  template<typename S, typename V, typename K, typename FP> class GvmKeyer;
  template<typename S, typename V, typename K, typename FP> class GvmDefaultKeyer;
  
  template<typename S, typename V, typename K, typename FP> class GvmResult;
  
  template<typename V, typename FP, int D> class GvmVectorSpace;
}
