//
//  GvmClusterPairs.hpp
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

// Maintains a heap of cluster pairs.

#import "GvmCommon.hpp"

#import "GvmClusterPair.hpp"

namespace Gvm {

  // S
  // Specific subclass that implements GvmSpace
  
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
  class GvmClusterPairs {
  public:

    static inline
    int ushift_left(int n) {
#if defined(DEBUG)
      assert(n >= 0);
#endif // DEBUG
      uint32_t un = (uint32_t) n;
      un <<= 1;
      return un;
    }
    
    static inline
    int ushift_right(int n) {
#if defined(DEBUG)
      assert(n >= 0);
#endif // DEBUG
      uint32_t un = (uint32_t) n;
      un >>= 1;
      return un;
    }
    
    // Pairs is an array of pointers to GvmClusterPair objects.
    // This data structure makes it easy to move an item
    // allocated on the heap around in the array without having
    // to copy the object data. The shared refs means that
    // GvmClusters can also hold refs to these same pair objects.
    
    std::vector<std::shared_ptr<GvmClusterPair<S,K,P>> > pairs;
    
    int size;
        
    GvmClusterPairs<S,K,P>(int initialCapacity)
    : size(0)
    {
      pairs.reserve(initialCapacity);
      for (int i=0; i < initialCapacity; i++) {
        pairs.push_back(nullptr);
      }
      return;
    }
    
    // Copy constructor
    
    GvmClusterPairs<S,K,P>(GvmClusterPairs<S,K,P> &that) {
      pairs = that.pairs;
      size = pairs.length;
    }
    
    bool add(std::shared_ptr<GvmClusterPair<S,K,P> > &pair) {
      int i = size;
      if (i >= pairs.size()) {
        grow(i + 1);
      }
      size = i + 1;
      if (i == 0) {
        pairs[0] = pair;
        pair.get()->index = 0;
      } else {
        heapifyUp(i, pair);
      }
      return true;
    }
    
    // add cluster pair and return ref to shared pair object that was just added
    
    std::shared_ptr<GvmClusterPair<S,K,P>>
    newSharedPair(GvmCluster<S,K,P> &c1, GvmCluster<S,K,P> &c2) {
      auto newPairPtr = std::make_shared<GvmClusterPair<S,K,P> >(c1, c2);
      return newPairPtr;
    }

    GvmClusterPair<S,K,P>* peek() {
      return size == 0 ? nullptr : pairs[0].get();
    }
    
    bool remove(std::shared_ptr<GvmClusterPair<S,K,P> > &pair) {
      int i = indexOf(pair);
      if (i == -1) return false;
      removeAt(i);
      return true;
    }
    
    void reprioritize(std::shared_ptr<GvmClusterPair<S,K,P> > &pair) {
      GvmClusterPair<S,K,P> &pairObj = *(pair.get());
      int i = indexOf(pair);
      if (i == -1) {
        assert(0);
      }
      pairObj.update();
      std::shared_ptr<GvmClusterPair<S,K,P> > *parent = (i == 0) ? nullptr : &pairs[ ushift_right(i - 1) ];
      if (parent != nullptr && parent->get()->value > pairObj.value) {
        heapifyUp(i, pair);
      } else {
        heapifyDown(i, pair);
      }
    }
    
    int getSize() {
      return size;
    }
    
    void clear() {
      for (int i = 0; i < size; i++) {
        GvmClusterPair<S,K,P> &e = pairs[i];
        e.index = -1;
        //pairs[i] = e;
      }
      size = 0;
    }
    
    void grow(int minCapacity) {
      if (minCapacity < 0) {
        assert(0); // can't grow, maximum number of elements exceeded
      }
      int oldCapacity = (int) pairs.size();
      int newCapacity = ((oldCapacity < 64)? ((oldCapacity + 1) * 2): ((oldCapacity / 2) * 3));
      // Note that capacity here is limited to 32 bit signed int range
      if (newCapacity < 0) newCapacity = std::numeric_limits<int32_t>::max();
      if (newCapacity < minCapacity) newCapacity = minCapacity;
      pairs.reserve(newCapacity);
    }
    
    int indexOf(std::shared_ptr<GvmClusterPair<S,K,P> > &pair) {
      return pair == nullptr ? -1 : pair->index;
    }
    
    void removeAt(int i) {
      int s = --size;
      if (s == i) {
        // removing last element
        pairs[i].get()->index = -1;
        pairs[i] = nullptr;
      } else {
        // Move pair object from one array slot to another
        std::shared_ptr<GvmClusterPair<S,K,P> > moved = pairs[s];
        pairs[s] = nullptr;
        moved.get()->index = -1;
        heapifyDown(i, moved);
        if (pairs[i] == moved) {
          heapifyUp(i, moved);
          if (pairs[i] != moved) {
            return;
          }
        }
      }
      return;
    }
    
    void heapifyUp(int k, std::shared_ptr<GvmClusterPair<S,K,P> > &pair) {
      auto pairValue = pair.get()->value;
      while (k > 0) {
        int parent = ushift_right(k - 1);
        std::shared_ptr<GvmClusterPair<S,K,P> > &e = pairs[parent];
        if (pairValue >= e.get()->value) break;
        pairs[k] = e;
        e.get()->index = k;
        k = parent;
      }
      pairs[k] = pair;
      pair.get()->index = k;
    }
    
    void heapifyDown(int k, std::shared_ptr<GvmClusterPair<S,K,P> > &pair) {
      auto pairValue = pair.get()->value;
      int half = ushift_right(size);
      while (k < half) {
        int child = ushift_left(k) + 1;
        std::shared_ptr<GvmClusterPair<S,K,P> > *c = &pairs[child];
        int right = child + 1;
        if (right < size && c->get()->value > pairs[right].get()->value) {
          child = right;
          c = &pairs[child];
        }
        if (pairValue <= c->get()->value) break;
        pairs[k] = *c;
        c->get()->index = k;
        k = child;
      }
      pairs[k] = pair;
      pair.get()->index = k;
    }
    
  }; // end class GvmClusterPairs

}
