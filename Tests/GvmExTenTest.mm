//
//  GvmExTenTest.mm
//  GvmTest
//
//  Created by Mo DeJong on 8/4/15.
//  Copyright (c) 2015 helpurock. All rights reserved.
//
//  Clustering test example with 10 points, a simple 2D scatter plot shows 3 primary
//  clusters all along a correlated line.

#import <Cocoa/Cocoa.h>
#import <XCTest/XCTest.h>

#import "Gvm.hpp"

#include <iostream>

using namespace std;
using namespace Gvm;

@interface GvmExTenTest : XCTestCase

@end

// Setup template parameters for 2D coordinates

# define FP double
# define ClusterVector GvmStdVector<FP,2>
# define ClusterVectorSpace GvmVectorSpace<ClusterVector,FP,2>
# define ClusterKey vector<ClusterVector>

@implementation GvmExTenTest

- (vector<ClusterVector>) getTestTenSampleVec
{
  double data2D[] = {
  0.3325312236041255,0.48738482998727056,
  0.34623985334890334,0.5347660877788399,
  0.2748244527993096,0.3467986203074948,
  0.7136742195009704,0.36177938722906483,
  0.6318292800942289,0.729730195802691,
  0.3470855716936217,0.552559573732038,
  0.3623743087637443,0.5248033439746583,
  0.6111643061119941,0.7422235950519993,
  0.31218064389129385,0.533100359437152,
  0.6533443188801747,0.7457430915431426
  };
  
  vector<ClusterVector> coordsVec;
  
  for (int i = 0; i < sizeof(data2D)/sizeof(double); i+= 2) {
    double x = data2D[i];
    double y = data2D[i+1];
    
    ClusterVector coords;
    
    coords[0] = x;
    coords[1] = y;
    
    coordsVec.push_back(coords);
  }
  
  assert(coordsVec.size() == 10);
  
  return coordsVec;
}

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testGvmTen {

  // The cluster "key" is a list of int values that correspond
  // to a certain coordinate.
  
  ClusterVectorSpace vspace;
  
  GvmClusters<ClusterVectorSpace, ClusterVector, ClusterKey, FP> clusters(vspace, 3);
  
  vector<ClusterVector> listOfPoints = [self getTestTenSampleVec];
  
  for ( ClusterVector & pt : listOfPoints ) {
    clusters.add(1, pt, nullptr);
  }
  
  // 3 clusters
  
  vector<GvmResult<ClusterVectorSpace, ClusterVector, ClusterKey, FP> > results = clusters.results();
  
  XCTAssert(results.size() == 3);
  
  if (false) {
  for ( auto & result : results ) {
    cout << "cluster: " << result.toString() << endl;
  }
  }
  
  auto & result0 = results[0];
  auto & result1 = results[1];
  auto & result2 = results[2];
  
  double cx, cy;
  
  // cluster 0: count 6, mass 6
  
  XCTAssert(result0.count == 6);
  
  // variance: 0.0057, mass 6
  
  XCTAssert(result0.getMass() == 6);
  XCTAssert(int(round(result0.getVariance() * 1000.0)) == 6);
  
  // cluster center of mass
  // [0.329206009016833, 0.49656880253624225]
  cx = result0.point[0];
  cy = result0.point[1];
  XCTAssert(int(round(cx * 100.0)) == 33);
  XCTAssert(int(round(cy * 100.0)) == 50);
  
  // cluster 1: count 1
  // getVariance: 0.0, mass: 1.0
  
  XCTAssert(result1.count == 1);
  XCTAssert(result1.getMass() == 1);
  XCTAssert(result1.getVariance() == 0.0);
  
  // cluster center of mass
  // [0.7136742195009704, 0.36177938722906483]
  cx = result1.point[0];
  cy = result1.point[1];
  XCTAssert(int(round(cx * 100.0)) == 71);
  XCTAssert(int(round(cy * 100.0)) == 36);
  
  // cluster 2: count 3
  // getVariance: 0.0003437751367911061E, mass: 3.0
  
  XCTAssert(result2.count == 3);
  XCTAssert(result2.getMass() == 3);
  XCTAssert(int(round(result2.getVariance() * 10000.0)) == 3);
  
  // cluster center of mass
  // [0.6321126350287992, 0.7392322941326109]
  cx = result2.point[0];
  cy = result2.point[1];
  XCTAssert(int(round(cx * 100.0)) == 63);
  XCTAssert(int(round(cy * 100.0)) == 74);
}

#undef FP
#undef ClusterVector
#undef ClusterVectorSpace
#undef ClusterKey

@end
