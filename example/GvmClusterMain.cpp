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

// Given a pixel represented as a 32 bit integer convert to a vector
// of floating points representation and store in outVec.

template<typename V, typename P>
static inline
void convertPoint(int32_t pixel, V &outVec)
{
  uint32_t B = pixel & 0xFF;
  uint32_t G = (pixel >> 8) & 0xFF;
  uint32_t R = (pixel >> 16) & 0xFF;
  
  P x = B;
  P y = G;
  P z = R;
  
  outVec[0] = x;
  outVec[1] = y;
  outVec[2] = z;
  
  return;
}

// Scan input values to make sure there are no duplicate pixels

#if defined(DEBUG)

void checkForDuplicates(vector<uint32_t> &inPixelsVec, uint32_t allowedDup)
{
  // In DEBUG mode, use unordered_map to check for duplicate pixels in the input.
  // Note that BGRA pixels will be ignored since different alpha values will be
  // treated as the same point as fully opaque. The caller must take care to
  // implement this alpha value filtering before invoking this method.
  
  unordered_map<uint32_t, uint32_t> uniquePixelMap;
  
  for ( uint32_t pixel : inPixelsVec ) {
    if (pixel != allowedDup && uniquePixelMap.count(pixel) > 0) {
      fprintf(stdout, "input contains duplicate BGR pixel 0x%08X\n", pixel);
      exit(1);
    }
    
    uniquePixelMap[pixel] = 0;
  }
  
  return;
}

#endif // DEBUG

// Read YUV pixels in BGRA order and return in a vector of uint32_t

// Load points from a file

vector<uint32_t> loadPixelsFromBGR(string filename)
{
  vector<uint32_t> allPoints;
  
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
  
  for (; numBytes > 0 ; numBytes -= 4) {
    uint32_t pixel;
    
    int numRead = (int)fread(&pixel, sizeof(pixel), 1, fp);
    if (numRead != 1) {
      cerr << "could not read whole pixel from YUV input at offset " << ftell(fp) << endl;
      exit(1);
    }
    
    allPoints.push_back(pixel);
  }
  
  fclose(fp);
  
  return allPoints;
}

int main(int argc, char **argv) {
  
  if (argc != 2) {
    fprintf(stderr, "usage gvm_cluster_yuv YUV\n");
    exit(1);
  }
  
  string filename = argv[1];

  // Using float instead of double cuts memory usage down just a bit, like 10%
  
  typedef double FP;
//  typedef float FP;
  
  // ClusterVector and ClusterVspace define the low level fixed size
  // vector of values that represents the point values.
  
  typedef GvmStdVector<FP,3> ClusterVector;
  typedef GvmVectorSpace<ClusterVector,FP,3> ClusterVectorSpace;

  // A "key" is a vector of points in one specific cluster.
  
  typedef vector<uint32_t> ClusterKey;
  
  vector<uint32_t> allPixels = loadPixelsFromBGR(filename);
  
#if defined(DEBUG)
  checkForDuplicates(allPixels, 0xFF000000);
#endif // DEBUG

  cout << "read " << allPixels.size() << " pixels from " << filename << endl;
  
  ClusterVectorSpace vspace;
  
  //const int numClusters = 2048;
  //const int numClusters = 1024;
  //const int numClusters = 512; // 30 megs of ram, 8 sec CPU
  const int numClusters = 256; // 16 megs of ram, 3 sec of CPU
  //const int numClusters = 128; // 16 megs of ram, 1 sec of CPU
  
  GvmClusters<ClusterVectorSpace, ClusterVector, ClusterKey, FP> clusters(vspace, numClusters);
  
  GvmListKeyer<ClusterVectorSpace, ClusterVector, ClusterKey, FP> listKeyer;
  
  // Install key combiner for list of points, caller must manage ptr lifetime
  
  clusters.setKeyer(&listKeyer);
  
#if defined(DEBUG)
  if ((1)) {
    clusters.pointDebugOutput = fopen("clustering_point_debug.txt", "w");
  }
#endif // DEBUG
  
  // Insert each point into clusters. Each point is
  // associated with a list of points called a "key".
  
  ClusterVector pt;
  
  for ( uint32_t pixel : allPixels ) {
    // Key is a list of (list of points)
    
    convertPoint<ClusterVector,FP>(pixel, pt);
    
    if (false) {
    assert(pt.getDimensions() == 3);
    cout << "clustering point (B G R) (" << pt[0] << " " << pt[1] << " " << pt[2] << ")" << endl;
    }
    
    ClusterKey key;
    
    key.push_back(pixel);
    
    clusters.add(1, pt, &key);
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
    
    ClusterKey *clusterKeys = results[i].getKey();
    
    cout << "cluster " << clusterID << " contains " << clusterKeys->size() << " pixels" << endl;
    
    int pixelsWritten = 0;
    
    for ( uint32_t pixel : *clusterKeys ) {
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
  
  // In the combination phase, pixels split into clusters are combined based on calculation
  // of distances from one cluster center to the next.
  
  return 0;
}
