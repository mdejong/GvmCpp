//
//  GvmClusterMain.cpp
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

#include <iostream>

#include <unordered_map>

#import "Gvm.hpp"

using namespace std;
using namespace Gvm;

// Load points from a file

template<typename V, typename P>
vector<V> loadPointsFromBGR(string filename)
{
  vector<V> allPoints;
  
  FILE *fp = fopen(filename.c_str(), "rb");
  
  fseek(fp, 0, SEEK_END);

  int numBytes = (int) ftell(fp);

  if (numBytes == 0) {
    cerr << "empty input YUV file contains zero BGRA pixels" << endl;
    exit(1);
  }
  
  if ((numBytes % sizeof(uint32_t)) != 0) {
    cerr << "input YUV must be a file with a whole number of BGRA pixels" << endl;
    exit(1);
  }

  fseek(fp, 0, SEEK_SET);
  
  // In DEBUG mode, use unordered_map to check for duplicate pixels in the input
  
#if defined(DEBUG)
  unordered_map<uint32_t, uint32_t> uniquePixelMap;
#endif // DEBUG
  
  for (; numBytes > 0 ; numBytes -= 4) {
    uint32_t pixel;
    
    int numRead = (int)fread(&pixel, sizeof(pixel), 1, fp);
    if (numRead != 1) {
      cerr << "could not read whole pixel from YUV input at offset " << ftell(fp) << endl;
      exit(1);
    }
    
    uint32_t B = pixel & 0xFF;
    uint32_t G = (pixel >> 8) & 0xFF;
    uint32_t R = (pixel >> 16) & 0xFF;
    
#if defined(DEBUG)
    uint32_t pixelNoAlpha = (R << 16) | (G << 8) | B;
    
    if (uniquePixelMap.count(pixelNoAlpha) > 0) {
      cerr << "input contains duplicate BGR pixel" << endl;
      exit(1);
    }
#endif // DEBUG
    
    P x = B;
    P y = G;
    P z = R;
    
    V coordsVec;
    
    coordsVec[0] = x;
    coordsVec[1] = y;
    coordsVec[2] = z;
    
    allPoints.push_back(coordsVec);
  }
  
  fclose(fp);
  
  return allPoints;
}

int main(int argc, char **argv) {
  
  string filename = "/Users/modejong/Development/ImageCompression/GVMCluster/Lenna_int_order.yuv";

  typedef double FP;
//    typedef float FP;
  
  // ClusterVector and ClusterVspace define the low level fixed size
  // vector of values that represents the point values.
  
  typedef GvmStdVector<FP,3> ClusterVector;
  typedef GvmVectorSpace<ClusterVector,FP,3> ClusterVectorSpace;

  // A "key" is a vector of points in one specific cluster.
  
  typedef vector<ClusterVector> ClusterKey;
  
  vector<ClusterVector> allPoints = loadPointsFromBGR<ClusterVector,FP>(filename);

  cout << "read " << allPoints.size() << " pixels from " << filename << endl;
  
  ClusterVectorSpace vspace;
  
  const int numClusters = 2048;
  
  GvmClusters<ClusterVectorSpace, ClusterVector, ClusterKey, FP> clusters(vspace, numClusters);
  
  GvmListKeyer<ClusterVectorSpace, ClusterVector, ClusterKey, FP> listKeyer;
  
  // Install key combiner for list of points, caller must manage ptr lifetime
  
  clusters.setKeyer(&listKeyer);
  
  // Insert each point into clusters. Each point is
  // associated with a list of points called a "key".
  // This key object lifetime must be managed by the caller
  // for performance reasons.
  
  vector<ClusterKey> allKeys;
  allKeys.reserve(allPoints.size());
  
  for ( ClusterVector & pt : allPoints ) {
    // Key is a list of (list of points)
    if (false) {
    assert(pt.getDimensions() == 3);
    cout << "clustering point (B G R) (" << pt[0] << " " << pt[1] << " " << pt[2] << ")" << endl;
    }
    
    allKeys.push_back(ClusterKey ());
    ClusterKey *key = &allKeys[allKeys.size() - 1];
    key->push_back(pt);
    
    clusters.add(1, pt, key);
  }
  
  cout << "generated " << clusters.clusters.size() << " clusters" << endl;
  
  // Emit clustered results as YUV data padded out to 256 with zeros
  
  // vector<GvmResult>
  
  int clusteri = 0;
  
  vector<GvmResult<ClusterVectorSpace, ClusterVector, ClusterKey, FP>> results = clusters.results();
  
  for ( auto & result : results ) {
    cout << "cluster[" << clusteri << "]: " << result.toString() << endl;
    clusteri++;
  }
  
  // Write clustered pixels as YUV output
  
  int totalPixelsWritten = 0;
  
  string outYuvFilename = "out.yuv";
  
  FILE *outFP = fopen(outYuvFilename.c_str(), "wb");
  
  for (int i = 0; i < results.size(); i++) {
    int clusterID = i; // keep to byte range
    
    ClusterKey *allKeys = results[i].getKey();
    
    cout << "cluster " << clusterID << " contains " << allKeys->size() << " pixels" << endl;
    
    int pixelsWritten = 0;
    
    for ( ClusterVector &pt : *allKeys ) {
      FP x = pt[0];
      FP y = pt[1];
      FP z = pt[2];
      
      if (x < 0 || x > 255) {
        assert(0);
      }
      if (y < 0 || y > 255) {
        assert(0);
      }
      if (z < 0 || z > 255) {
        assert(0);
      }
      
      uint32_t B = int(x);
      uint32_t G = int(y);
      uint32_t R = int(z);
      
      uint32_t pixel = (R << 16) | (G << 8) | B;

      int numWritten = (int)fwrite(&pixel, sizeof(uint32_t), 1, outFP);
      assert(numWritten == 1);
      pixelsWritten++;
    }
    
    totalPixelsWritten += pixelsWritten;
    
    if (true) {
      // Pad each cluster out to 256
      
      int numPointsInCluster = pixelsWritten;
      
      int over = numPointsInCluster % 256;
      int under = 0;
      
      if (over == 0) {
        // Cluster of exactly 256 pixels, emit 256 zeros to indicate this case.
        under = 256;
      } else {
        under = 256 - over;
      }
      
      for (int j = 0; j < under; j++) {
        uint32_t pixel = 0;
        int numWritten = (int)fwrite(&pixel, sizeof(uint32_t), 1, outFP);
        assert(numWritten == 1);
        totalPixelsWritten++;
      }
    }
  }

  fclose(outFP);
  
  cout << "wrote " << totalPixelsWritten << " total pixels to " << outYuvFilename << endl;
  
  int totalRowsOf256 = totalPixelsWritten/256;
  if ((totalPixelsWritten % 256) != 0) {
    assert(0);
  }

  cout << "wrote 256 x " << totalRowsOf256 << endl;
  
  cout << "as " << results.size() << " number of clusters" << endl;
  
  return 0;
}
