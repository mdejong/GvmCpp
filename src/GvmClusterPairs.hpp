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
    // to copy the object data which is critical for performance.
    
    GvmClusterPair<S,V,K,FP> **pairs;
    
    // Cluster pairs are allocated as a solid block of (N * N) instances
    // so that these pointers can be sorted and rearranged without use
    // of shared_ptr for each cluster pair object.
    
    GvmClusterPair<S,V,K,FP> *pairsArray;
    
    int size;
    
    // The size of the pairs allocation. This size is defined at object
    // construction time and cannot be changed during object lifetime
    // for performance reasons.
    
    int capacity;
    
    // The number of pairsArray values that have been used.
    
    int pairsUsed;
    
    GvmClusterPairs<S,V,K,FP>(int inCapacity)
    : size(0), capacity(inCapacity), pairsUsed(0)
    {
      // For N clusters, allocate solid block of (N * N) cluster pairs,
      // this allocation represents a significant amount of the memory
      // used by the library.
      
      assert(capacity > 0);
      
      pairsArray = new GvmClusterPair<S,V,K,FP>[capacity];
      assert(pairsArray);
      
      if ((0)) {
        fprintf(stdout, "alloc pairsArray of size %d bytes : 0x%p\n", (int)(capacity * sizeof(GvmClusterPair<S,V,K,FP>)), pairsArray);
      }
      
      // pairs is an array of pointers into pairsArray
      // initialized to nullptr.
      
      pairs = new GvmClusterPair<S,V,K,FP>*[capacity]();
      assert(pairs);
      
      if ((0)) {
        fprintf(stdout, "alloc pairs of size %d bytes : 0x%p\n", (int)(capacity * sizeof(GvmClusterPair<S,V,K,FP>*)), pairs);
      }
      
      return;
    }
    
    ~GvmClusterPairs<S,V,K,FP>() {
      if ((0)) {
        fprintf(stdout, "dealloc pairs 0x%p\n", pairs);
      }
      
      delete [] pairs;
      
      if ((0)) {
        fprintf(stdout, "dealloc pairsArray 0x%p\n", pairsArray);
      }
      
      delete [] pairsArray;
    }
    
    // Copy constructor explicitly deleted

    GvmClusterPairs<S,V,K,FP>(GvmClusterPairs<S,V,K,FP> &that) = delete;
    GvmClusterPairs<S,V,K,FP>(const GvmClusterPairs<S,V,K,FP> &that) = delete;
    
    // Operator= explicitly deleted
    
    GvmClusterPairs<S,V,K,FP>& operator=(GvmClusterPairs<S,V,K,FP>& x) = delete;
    GvmClusterPairs<S,V,K,FP>& operator=(const GvmClusterPairs<S,V,K,FP>& x) = delete;
    
    // add() should be passed a pair pointer returned by newSharedPair.
    
    void add(GvmClusterPair<S,V,K,FP> *pair) {
      int i = size;
      
      // Note that grow() logic is not supported here since the object
      // uses a fixed max size for pairs so that this add() method
      // can execute a more optimal execution path.
      
#if defined(DEBUG)
      if (i >= capacity) {
        assert(0);
      }
#endif // DEBUG
      
      size = i + 1;
      if (i == 0) {
        pairs[0] = pair;
        pair->index = 0;
      } else {
        heapifyUp(i, pair);
      }
      return;
    }
    
    // add cluster pair and return ref to shared pair object that was just added
    
    GvmClusterPair<S,V,K,FP>*
    newSharedPair(GvmCluster<S,V,K,FP> &c1, GvmCluster<S,V,K,FP> &c2) {
      assert(pairsUsed <= (capacity-1));
      GvmClusterPair<S,V,K,FP> *pairPtr = &pairsArray[pairsUsed];
      pairsUsed++;
      pairPtr->set(&c1, &c2);
      return pairPtr;
    }

    GvmClusterPair<S,V,K,FP>* peek() {
      return size == 0 ? nullptr : pairs[0];
    }
    
    bool remove(GvmClusterPair<S,V,K,FP> *pair) {
      int i = indexOf(pair);
      if (i == -1) return false;
      removeAt(i);
      return true;
    }
    
    void reprioritize(GvmClusterPair<S,V,K,FP> *pair) {
      int i = indexOf(pair);
#if defined(DEBUG)
      if (i == -1) {
        assert(0);
      }
#endif // DEBUG
      pair->update();
      GvmClusterPair<S,V,K,FP> *parent = (i == 0) ? nullptr : pairs[ ushift_right(i - 1) ];
      if (parent != nullptr && parent->value > pair->value) {
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
        GvmClusterPair<S,V,K,FP> *e = pairs[i];
        e->index = -1;
        //pairs[i] = e;
      }
      size = 0;
    }
    
    int indexOf(GvmClusterPair<S,V,K,FP> *pair) {
#if defined(DEBUG)
      assert(pair != nullptr);
#endif // DEBUG
      return pair->index;
    }
    
    void removeAt(int i) {
      int s = --size;
      if (s == i) {
        // removing last element
        pairs[i]->index = -1;
        pairs[i] = nullptr;
      } else {
        // Move pair object from one array slot to another
        GvmClusterPair<S,V,K,FP> *moved = pairs[s];
        pairs[s] = nullptr;
        moved->index = -1;
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
    
    void heapifyUp(int k, GvmClusterPair<S,V,K,FP> *pair) {
      auto pairValue = pair->value;
      while (k > 0) {
        int parent = ushift_right(k - 1);
        GvmClusterPair<S,V,K,FP> *e = pairs[parent];
        if (pairValue >= e->value) break;
        pairs[k] = e;
        e->index = k;
        k = parent;
      }
      pairs[k] = pair;
      pair->index = k;
    }
    
    void heapifyDown(int k, GvmClusterPair<S,V,K,FP> *pair) {
      auto pairValue = pair->value;
      int half = ushift_right(size);
      while (k < half) {
        int child = ushift_left(k) + 1;
        GvmClusterPair<S,V,K,FP> *c = pairs[child];
        int right = child + 1;
        if (right < size && c->value > pairs[right]->value) {
          child = right;
          c = pairs[child];
        }
        if (pairValue <= c->value) break;
        pairs[k] = c;
        c->index = k;
        k = child;
      }
      pairs[k] = pair;
      pair->index = k;
    }
    
  }; // end class GvmClusterPairs

}
