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
  class GvmClusters {
  public:
    
    // statics
    
    // Helper method to avoid propagation of negative variances.
    //
    // var : a variance
    // return the variance clamped at zero
    
    static inline FP correct(FP var) {
      return var >= FP(0.0) ? var : FP(0.0);
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
    
    std::unique_ptr<GvmDefaultKeyer<S,V,K,FP> > defaultKeyerPtr;
    
    // The user can pass a custom keyer object to use
    // instead of the default keyer. The caller must
    // take care to manage the lifetime of the pointer.

    GvmKeyer<S,V,K,FP> *keyerPtr;
    
    // The clusters objects.

    std::vector<std::shared_ptr<GvmCluster<S,V,K,FP>> > clusters;
    
    // All possible cluster pairs.
    
    GvmClusterPairs<S,V,K,FP> pairs;
    
    // The number of points that have been added.
    
    int additions;
    
    // The current number of clusters.
    
    int count;
    
    // The number of clusters prior to reduction
    
    int bound;
    
#if defined(DEBUG)
    FILE *pointDebugOutput;
#endif // DEBUG
    
    // constructor
    
    GvmClusters<S,V,K,FP>(S inSpace, int inCapacity)
    :
    space(inSpace),
    capacity(inCapacity),
    defaultKeyerPtr(new GvmDefaultKeyer<S,V,K,FP>()),
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
#if defined(DEBUG)
      pointDebugOutput = nullptr;
#endif // DEBUG
    }
    
    // The keyer used to assign keys to clusters.
    
    GvmKeyer<S,V,K,FP>* getKeyer() {
      if (keyerPtr) {
        return keyerPtr;
      } else {
        return defaultKeyerPtr.get();
      }
    }
    
    // Setter for keyer property, use this method to define a new keyer instead of
    // using GvmDefaultKeyer. Note that nullptr cannot be passed to this method.
    
    void setKeyer(GvmKeyer<S,V,K,FP> *inKeyer) {
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
    // key : the key associated with a specific
    // point is typically a vector that contains
    // a value specific to the point. The lifetime
    // of the key object is not managed by the library,
    // the caller will typically pass in a tmp value
    // allocated on the stack. The key can be nullptr.
    
    void add(const FP m, V &pt, K *key) {
      if (m == FP(0.0)) return; //nothing to do
      
      GvmKeyer<S,V,K,FP>* const keyer = getKeyer();
      
      if (count < capacity) { //shortcut
        //TODO should prefer add if var comes to zero
        
#if defined(DEBUG)
        if (pointDebugOutput) {
          std::string ptStr = pt.toString();
          fprintf(pointDebugOutput, "add to cluster[%d] for point %s\n", additions, ptStr.c_str());
        }
#endif // DEBUG
        
        auto newClusterPtr = std::make_shared<GvmCluster<S,V,K,FP> >(*this);
#if defined(DEBUG)
        assert(clusters[additions] == nullptr);
#endif // DEBUG
        clusters[additions] = newClusterPtr;
        GvmCluster<S,V,K,FP> &cluster = *(newClusterPtr.get());
        cluster.set(m, pt);
        addPairs();
        cluster.setKey(keyer->addKey(cluster, key));
        count++;
        bound = count;
      } else {
        //identify cheapest merge
        GvmClusterPair<S,V,K,FP> *mergePairPtr = pairs.peek();
        FP mergeT = mergePairPtr == nullptr ? std::numeric_limits<FP>::max() : mergePairPtr->value;
        
#if defined(DEBUG)
        if (pointDebugOutput) {
          std::string ptStr = pt.toString();
          fprintf(pointDebugOutput, "merge threshold is %0.16f with %d clusters\n", mergeT, count);
        }
#endif // DEBUG
        
        //find cheapest addition
        GvmCluster<S,V,K,FP> *additionCPtr = nullptr;
        FP additionT = std::numeric_limits<FP>::max();
#if defined(DEBUG)
        int additionI = 0;
#endif // DEBUG
        for (int i = 0; i < clusters.size(); i++) {
          auto &clusterSharedPtr = clusters[i];
          GvmCluster<S,V,K,FP> *clusterPtr = clusterSharedPtr.get();
          FP t = clusterPtr->test(m, pt);
          if (t < additionT) {
            additionCPtr = clusterPtr;
            additionT = t;
#if defined(DEBUG)
            additionI = i;
#endif // DEBUG
          }
        }
        if (additionT <= mergeT) {
#if defined(DEBUG)
          if (pointDebugOutput) {
            std::string ptStr = pt.toString();
            fprintf(pointDebugOutput, "cheapest add is %0.16f for cluster[%d] and point %s\n", additionT, additionI, ptStr.c_str());
          }
#endif // DEBUG
          
          //choose addition
          GvmCluster<S,V,K,FP> &additionC = *additionCPtr;
          additionC.add(m, pt);
          updatePairs(additionC);
          additionC.setKey(keyer->addKey(additionC, key));
        } else {
#if defined(DEBUG)
          if (pointDebugOutput) {
            std::string ptStr = pt.toString();
            fprintf(pointDebugOutput, "cheapest merge is %0.16f for cluster[%d] and point %s\n", additionT, additionI, ptStr.c_str());
          }
#endif // DEBUG
          
          //choose merge
          GvmCluster<S,V,K,FP> *c1 = mergePairPtr->c1;
          GvmCluster<S,V,K,FP> *c2 = mergePairPtr->c2;
          if (c1->m0 < c2->m0) {
            c1 = c2;
            c2 = mergePairPtr->c1;
#if defined(DEBUG)
            if (pointDebugOutput) {
              std::string ptStr = pt.toString();
              fprintf(pointDebugOutput, "merge c2 <- c1 : N keys %d <- %d\n", (int)c1->keyVec.size(), (int)c2->keyVec.size());
            }
#endif // DEBUG
          } else {
#if defined(DEBUG)
            if (pointDebugOutput) {
              std::string ptStr = pt.toString();
              fprintf(pointDebugOutput, "merge c1 <- c2: N keys %d <- %d\n", (int)c2->keyVec.size(), (int)c1->keyVec.size());
            }
#endif // DEBUG
          }
          c1->setKey(keyer->mergeKeys(*c1, *c2));
          c1->add(*c2);
          updatePairs(*c1);
          c2->set(m, pt);
          updatePairs(*c2);
          //TODO should this pass through a method on keyer?
          c2->setKey(nullptr);
          c2->setKey(keyer->addKey(*c2, key));
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
    
    void reduce(FP maxVar, int minClusters) {
      assert(minClusters >= 0);
      if (count <= minClusters) return; //nothing to do
      
      FP totalVar = FP(0.0);
      FP totalMass = FP(0.0);
      for (int i = 0; i < count; i++) {
        auto &clusterSharedPtr = clusters[i];
        GvmCluster<S,V,K,FP> &cluster = *(clusterSharedPtr.get());
        totalVar += cluster.var;
        totalMass += cluster.m0;
      }
      
      GvmKeyer<S,V,K,FP>* const keyer = getKeyer();
      while (count > minClusters) {
        if (count == 1) {
          //remove the last cluster
          for (int i = 0; i < bound; i++) {
            auto &clusterSharedPtr = clusters[i];
            GvmCluster<S,V,K,FP> &c = *(clusterSharedPtr.get());
            if (!c.removed) {
              c.removed = true;
              break;
            }
          }
        } else {
          GvmClusterPair<S,V,K,FP> *mergePair = pairs.peek();
          assert(mergePair);
          GvmCluster<S,V,K,FP> *c1 = mergePair->c1;
          GvmCluster<S,V,K,FP> *c2 = mergePair->c2;
          
          if (c1->m0 < c2->m0) {
            c1 = c2;
            c2 = mergePair->c1;
          }
          if (maxVar >= FP(0.0)) {
            FP diff = c1->test(*c2) - c1->var - c2->var;
            totalVar += diff;
            if (totalVar/totalMass > maxVar) break; //stop here, we are going to exceed maximum
          }
          c1->setKey(keyer->mergeKeys(*c1, *c2));
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
          GvmCluster<S,V,K,FP> &cluster = *(clusterSharedPtr.get());
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
          bool lose = pair->c1->removed || pair->c2->removed;
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
    
    std::vector<GvmResult<S,V,K,FP>> results() {
      std::vector<GvmResult<S,V,K,FP>> list;
      for (int i = 0; i < count; i++) {
        auto &clusterSharedPtr = clusters[i];
        auto &cluster = *(clusterSharedPtr.get());
        //TODO exclude massless clusters?
        list.push_back(GvmResult<S,V,K,FP>(cluster));
      }
      return list;
    }
    
    // private utility methods
    
    //assumes that count not yet incremented
    //assumes last cluster is the one to add pairs for
    //assumes pairs are contiguous
    void addPairs() {
      GvmCluster<S,V,K,FP> &cj = *(clusters[count].get());
      int c = count - 1; //index at which new pairs registered for existing clusters
      for (int i = 0; i < count; i++) {
        GvmCluster<S,V,K,FP> &ci = *(clusters[i].get());
        auto pair = pairs.newSharedPair(ci, cj);
        ci.pairs[c] = pair;
        cj.pairs[i] = pair;
        pairs.add(pair);
      }
    }

    //does not assume pairs are contiguous
    void updatePairs(GvmCluster<S,V,K,FP> & cluster) {
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
          if (pair->c1->removed || pair->c2->removed) continue;
          this->pairs.reprioritize(pair);
        }
      }
    }

    //does not assume pairs are contiguous
    //leaves pairs in cluster pair lists
    //these are tidied when everything is made contiguous again
    void removePairs(GvmCluster<S,V,K,FP> & cluster) {
      auto &pairs = cluster.pairs;
      for (int i = 0; i < bound-1; i++) {
        auto &pair = pairs[i];
        if (pair->c1->removed || pair->c2->removed) continue;
        this->pairs.remove(pair);
      }
      
    }
    
  }; // end class GvmClusters

}
