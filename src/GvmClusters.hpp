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
#import "GvmClusterPairs.hpp"

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
    
    // FIXME: make defaultKeyerPtr a member object instead
    // of using unique_ptr here.
    
    // The default keyer is implicitly defined and is
    // used unless setKeyer() is invoked to make
    // this cluster use a custom keyer.
    
    std::unique_ptr<GvmDefaultKeyer<S,K,P> > defaultKeyerPtr;
    
    // The user can pass a custom keyer object to use
    // instead of the default keyer. The caller must
    // take care to manage the lifetime of the pointer.

    GvmKeyer<S,K,P> *keyerPtr;
    
    // The clusters objects.

    std::vector<std::shared_ptr<GvmCluster<S,K,P>> > clusters;
    
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
    keyerPtr(nullptr),
    pairs(capacity * (capacity-1) / 2),
    additions(0),
    count(0),
    bound(0)
    {
      assert(inCapacity > 0);
      clusters.reserve(capacity);
      for (int i=0; i < capacity; i++) {
        clusters.push_back(nullptr);
      }
    }
    
    // The keyer used to assign keys to clusters.
    
    GvmKeyer<S,K,P>* getKeyer() {
      if (keyerPtr) {
        return keyerPtr;
      } else {
        return defaultKeyerPtr.get();
      }
    }
    
    // Setter for keyer property, use this method to define a new keyer instead of
    // using GvmDefaultKeyer. Note that nullptr cannot be passed to this method.
    
    void setKeyer(GvmKeyer<S,K,P> *inKeyer) {
      keyerPtr = inKeyer;
      if (keyerPtr) {
      } else {
        assert(0);
      }
    }
    
    // Invoke this method to reset the keyer to the default keyer.
    // default keyer will be used again
    
    void resetKeyer() {
      keyerPtr = nullptr;
    }
    
    int getCapacity() {
      return capacity;
    }
    
    S getSpace() {
      return space;
    }
    
    // Removes all clusters and clustered points but retains the keyer.
    
    void clear() {
      for (int i=0; i < capacity; i++) {
        clusters[i] = nullptr;
      }
      pairs.clear();
      additions = 0;
      count = 0;
      bound = 0;
    }
    
    // Adds a point to be clustered.
    //
    // m : the mass at the point
    // pt : the coordinates of the point
    // key : the key assigned to the point (can be nullptr).
    // Note that this ref is not a unique_ptr
    // or a shared_ptr, the lifetime of this
    // pointer must be managed by the caller.
    
    void add(double m, std::vector<P> &pt, K* key) {
      if (m == 0.0) return; //nothing to do
      if (count < capacity) { //shortcut
        //TODO should prefer add if var comes to zero
        
        auto newClusterPtr = std::make_shared<GvmCluster<S,K,P> >(*this);
#if defined(DEBUG)
        assert(clusters[additions] == nullptr);
#endif // DEBUG
        clusters[additions] = newClusterPtr;
        GvmCluster<S,K,P> &cluster = *(newClusterPtr.get());
        cluster.set(m, pt);
        addPairs();
        cluster.key = getKeyer()->addKey(cluster, key);
        count++;
        bound = count;
      } else {
        //identify cheapest merge
        GvmClusterPair<S,K,P> *mergePairPtr = pairs.peek();
        double mergeT = mergePairPtr == nullptr ? std::numeric_limits<double>::max() : mergePairPtr->value;
        //find cheapest addition
        GvmCluster<S,K,P> *additionCPtr = nullptr;
        double additionT = std::numeric_limits<double>::max();
        for (int i = 0; i < clusters.size(); i++) {
          auto &clusterSharedPtr = clusters[i];
          GvmCluster<S,K,P> *clusterPtr = clusterSharedPtr.get();
          double t = clusterPtr->test(m, pt);
          if (t < additionT) {
            additionCPtr = clusterPtr;
            additionT = t;
          }
        }
        if (additionT <= mergeT) {
          //chose addition
          GvmCluster<S,K,P> &additionC = *additionCPtr;
          additionC.add(m, pt);
          updatePairs(additionC);
          additionC.key = getKeyer()->addKey(additionC, key);
        } else {
          //choose merge
          GvmClusterPair<S,K,P> &mergePair = *mergePairPtr;
          GvmCluster<S,K,P> *c1 = &mergePair.c1;
          GvmCluster<S,K,P> *c2 = &mergePair.c2;
          if (c1->m0 < c2->m0) {
            c1 = c2;
            c2 = &mergePair.c1;
          }
          c1->key = getKeyer()->mergeKeys(*c1, *c2);
          c1->add(*c2);
          updatePairs(*c1);
          c2->set(m, pt);
          updatePairs(*c2);
          //TODO should this pass through a method on keyer?
          c2->key = nullptr;
          c2->key = getKeyer()->addKey(*c2, key);
        }
      }
      additions++;
      
      return;
    }
    
    // Collapses the number of clusters subject to constraints on the maximum
    // permitted variance, and the least number of clusters. This method may be
    // called at any time, including between calls to add().
    //
    // maxVar : an upper bound on the global variance that may not be exceeded
    // by merging clusters
    // minClusters : a lower bound on the the number of clusters that may not be
    // exceeded by merging clusters
    
    void reduce(double maxVar, int minClusters) {
      assert(minClusters >= 0);
      if (count <= minClusters) return; //nothing to do
      
      double totalVar = 0.0;
      double totalMass = 0.0;
      for (int i = 0; i < count; i++) {
        auto &clusterSharedPtr = clusters[i];
        GvmCluster<S,K,P> &cluster = *(clusterSharedPtr.get());
        totalVar += cluster.var;
        totalMass += cluster.m0;
      }
      
      while (count > minClusters) {
        if (count == 1) {
          //remove the last cluster
          for (int i = 0; i < bound; i++) {
            auto &clusterSharedPtr = clusters[i];
            GvmCluster<S,K,P> &c = *(clusterSharedPtr.get());
            if (!c.removed) {
              c.removed = true;
              break;
            }
          }
        } else {
          GvmClusterPair<S,K,P> *mergePair = pairs.peek();
          assert(mergePair);
          GvmCluster<S,K,P> *c1 = &mergePair->c1;
          GvmCluster<S,K,P> *c2 = &mergePair->c2;
          
          if (c1->m0 < c2->m0) {
            c1 = c2;
            c2 = &mergePair->c1;
          }
          if (maxVar >= 0.0) {
            double diff = c1->test(*c2) - c1->var - c2->var;
            totalVar += diff;
            if (totalVar/totalMass > maxVar) break; //stop here, we are going to exceed maximum
          }
          c1->key = getKeyer()->mergeKeys(*c1, *c2);
          c1->add(*c2);
          updatePairs(*c1);
          removePairs(*c2);
          c2->removed = true;
        }
        count--;
      }
      //iterate over clusters and remove dead clusters
      {
        int j = 0;
        for (int i = 0; i < bound;) {
          auto &clusterSharedPtr = clusters[i];
          GvmCluster<S,K,P> &cluster = *(clusterSharedPtr.get());
          bool lose = cluster.removed;
          if (lose) {
            i++;
          } else {
            if (i != j) {
             clusters[j] = clusters[i];
            }
            i++;
            j++;
          }
        }
        for (; j < bound; j++) {
          clusters[j] = nullptr;
        }
      }
      //iterate over cluster pairs and remove dead pairs
      for (int i = 0; i < count; i++) {
        auto &clusterSharedPtr = clusters[i];
        auto &cluster = *(clusterSharedPtr.get());
        auto &pairs = cluster.pairs;
        int k = 0;
        for (int j = 0; j < bound-1;) {
          auto &pair = pairs[j];
          bool lose = pair.get()->c1.removed || pair.get()->c2.removed;
          if (lose) {
            j++;
          } else {
            if (j != k) {
              pairs[k] = pairs[j];
            }
            k++;
            j++;
          }
        }
        for (; k < bound; k++) {
          pairs[k] = nullptr;
        }
      }
      bound = count;
    }
    
    // Obtains the clusters for the points added. This method may be called
    // at any time, including between calls to add().
    //
    // return the result of clustering the points thus far added
    
    std::vector<GvmResult<S,K,P>> results() {
      std::vector<GvmResult<S,K,P>> list;
      for (int i = 0; i < count; i++) {
        auto &clusterSharedPtr = clusters[i];
        auto &cluster = *(clusterSharedPtr.get());
        //TODO exclude massless clusters?
        list.push_back(GvmResult<S,K,P>(cluster));
      }
      return list;
    }
    
    // private utility methods
    
    //assumes that count not yet incremented
    //assumes last cluster is the one to add pairs for
    //assumes pairs are contiguous
    void addPairs() {
      GvmCluster<S,K,P> &cj = *(clusters[count].get());
      int c = count - 1; //index at which new pairs registered for existing clusters
      for (int i = 0; i < count; i++) {
        GvmCluster<S,K,P> &ci = *(clusters[i].get());
        auto &pair = pairs.add(ci, cj);
        ci.pairs[c] = pair;
        cj.pairs[i] = pair;
      }
    }

    //does not assume pairs are contiguous
    void updatePairs(GvmCluster<S,K,P> & cluster) {
      auto &pairs = cluster.pairs;
      //accelerated path
      if (count == bound) {
        int limit = count - 1;
        for (int i = 0; i < limit; i++) {
          this->pairs.reprioritize(pairs[i]);
        }
      } else {
        int limit = bound - 1;
        for (int i = 0; i < limit; i++) {
          auto &pair = pairs[i];
          if (pair.get()->c1.removed || pair.get()->c2.removed) continue;
          this->pairs.reprioritize(pair);
        }
      }
    }

    //does not assume pairs are contiguous
    //leaves pairs in cluster pair lists
    //these are tidied when everything is made contiguous again
    void removePairs(GvmCluster<S,K,P> & cluster) {
      auto &pairs = cluster.pairs;
      for (int i = 0; i < bound-1; i++) {
        auto &pair = pairs[i];
        if (pair.get()->c1.removed || pair.get()->c2.removed) continue;
        this->pairs.remove(pair);
      }
      
    }
    
  }; // end class GvmClusters

}
