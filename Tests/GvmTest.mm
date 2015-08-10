//
//  GvmTest.m
//  GvmTest
//
//  Created by Mo DeJong on 8/4/15.
//  Copyright (c) 2015 helpurock. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <XCTest/XCTest.h>

#import "Gvm.hpp"

using namespace std;
using namespace Gvm;

@interface GvmTest : XCTestCase

@end

@implementation GvmTest

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testVectorSpace2D {
  // Create vector of double
  
  GvmVectorSpace<double, 2> vspace;
  XCTAssert(vspace.getDimensions() == 2, @"Pass");
  
  vector<double> vec = vspace.newOrigin();
  XCTAssert(vec.size() == 2, @"Pass");

  vector<double> vec1;
  vec1.push_back(1);
  vec1.push_back(2);
  
  vector<double> vec2 = vspace.newCopy(vec1);
  XCTAssert(vec2.size() == 2, @"Pass");
  XCTAssert(vec2[0] == 1, @"Pass");
  XCTAssert(vec2[1] == 2, @"Pass");
  
  double mag = vspace.magnitudeSqr(vec2);
  XCTAssert(mag == (2*2)+(1*1), @"Pass");
  
  vspace.setToOrigin(vec2);
  XCTAssert(vec2[0] == 0, @"Pass");
  XCTAssert(vec2[1] == 0, @"Pass");
  
  vspace.setTo(vec2, vec1);
  XCTAssert(vec2[0] == 1, @"Pass");
  XCTAssert(vec2[1] == 2, @"Pass");
  
  vspace.setToScaled(vec2, 2.0, vec1);
  XCTAssert(vec2[0] == 2, @"Pass");
  XCTAssert(vec2[1] == 4, @"Pass");

  vspace.setToScaledSqr(vec2, 2.0, vec1);
  XCTAssert(vec2[0] == (2.0 * 1.0 * 1.0), @"Pass");
  XCTAssert(vec2[1] == (2.0 * 2.0 * 2.0), @"Pass");
  
  vector<double> sum = vspace.newOrigin();
  vspace.add(sum, vec1);
  XCTAssert(sum[0] == 1, @"Pass");
  XCTAssert(sum[1] == 2, @"Pass");
  
  sum = vspace.newOrigin();
  vspace.addScaled(sum, 2.0, vec1);
  XCTAssert(sum[0] == 2, @"Pass");
  XCTAssert(sum[1] == 4, @"Pass");

  sum[0] = 2;
  sum[1] = 2;
  vspace.subtract(sum, vec1);
  XCTAssert(sum[0] == 1, @"Pass");
  XCTAssert(sum[1] == 0, @"Pass");
  
  sum[0] = 4;
  sum[1] = 4;
  vspace.subtractScaled(sum, 2.0, vec1);
  XCTAssert(sum[0] == 2, @"Pass");
  XCTAssert(sum[1] == 0, @"Pass");
  
  sum[0] = 8;
  sum[1] = 8;
  vspace.subtractScaledSqr(sum, 2.0, vec1);
  XCTAssert(sum[0] == 6, @"Pass");
  XCTAssert(sum[1] == 0, @"Pass");
  
  sum[0] = 8;
  sum[1] = 4;
  vspace.scale(sum, 0.5);
  XCTAssert(sum[0] == 4, @"Pass");
  XCTAssert(sum[1] == 2, @"Pass");
  
  sum[0] = 1;
  sum[1] = 2;
  vspace.square(sum);
  XCTAssert(sum[0] == 1, @"Pass");
  XCTAssert(sum[1] == 4, @"Pass");

  // dist between points
  // d = sqrt(D1 - D2 - D2)
  
  vec1[0] = 0;
  vec1[1] = 0;
  
  vec2[0] = 1;
  vec2[1] = 2;

  double d;
  d = vspace.distance(vec1, vec2);
  XCTAssert(d == sqrt(1.0+4.0), @"Pass");
  
  // variance 3 args

  vec1[0] = 1;
  vec1[1] = 2;
  
  vec2[0] = 1;
  vec2[1] = 4;

  double v;
  v = vspace.variance(2.0, vec1, vec2);
  XCTAssert(v == 2.5, @"Pass");
  
  // variance 5 args
  
  vec1[0] = 1;
  vec1[1] = 2;
  
  vector<double> sq1 = vspace.newOrigin();
  sq1[0] = 1;
  sq1[1] = 2*2;
  
  vec2[0] = 1;
  vec2[1] = 2;
  
  v = vspace.variance(2.0, vec1, sq1, 3.0, vec2);
  int ival = int(round(v * 10.0));
  XCTAssert(ival == 40, @"Pass");
  
  // variance 6 args
  
  vec1[0] = 1;
  vec1[1] = 2;
  
  sq1[0] = 1;
  sq1[1] = 2*2;
  
  vec2[0] = 2;
  vec2[1] = 3;
  
  vector<double> sq2 = vspace.newOrigin();
  
  sq2[0] = 2*2;
  sq2[1] = 3*3;
  
  v = vspace.variance(2.0, vec1, sq1, 3.0, vec2, sq2);
  ival = int(round(v * 10.0));
  XCTAssert(ival == 112, @"Pass");
  
  // Convert points to string
  string str = vspace.toString(vec1);
  XCTAssert(str == "1 2", @"Pass");
}

// Create instances of different objects

- (void)testGvmClusterObjects {
  
  // The cluster "key" is a list of int values that correspond
  // to a certain coordinate.
  
#define ClusterKey vector<vector<double> >
#define ClusterVspace GvmVectorSpace<double,2>
  
  ClusterVspace vspace;
  
  GvmClusters<ClusterVspace, ClusterKey, double> clusters(vspace, 256);
  
  GvmCluster<ClusterVspace, ClusterKey, double> c1(clusters);
  GvmCluster<ClusterVspace, ClusterKey, double> c2(clusters);
  
  GvmClusterPair<ClusterVspace, ClusterKey, double> cPair(c1, c2);
  
  XCTAssert(cPair.index == 0, @"Pass");

#undef ClusterKey
#undef ClusterVspace
}

- (void)testGvmClusterPair2D {

  // The cluster "key" is a list of int values that correspond
  // to a certain coordinate.
  
# define ClusterKey vector<vector<double> >
# define ClusterVspace GvmVectorSpace<double,2>
  
  ClusterVspace vspace;
  
  GvmClusters<ClusterVspace, ClusterKey, double> clusters(vspace, 256);

  // Generate list of 2D points
  
  double p1[] = { 0.0, 1.0 };
  double p2[] = { 1.0, 1.0 };
  double p3[] = { 2.0, 2.0 };
  
  vector<vector<double> > listOfPoints;
  
  vector<double> points;
  
  points.clear();
  points.push_back(p1[0]);
  points.push_back(p1[1]);
  listOfPoints.push_back(points);
  
  points.clear();
  points.push_back(p2[0]);
  points.push_back(p2[1]);
  listOfPoints.push_back(points);

  points.clear();
  points.push_back(p3[0]);
  points.push_back(p3[1]);
  listOfPoints.push_back(points);    
  
  // Insert each point into clusters. Each point is
  // associated with a list of points called a "key".
  // This key object lifetime must be managed by the caller
  // for performance reasons.
  
  vector<ClusterKey> allKeys;
  allKeys.reserve(listOfPoints.size());
  
  // Install key combiner for list of points, caller must manage ptr lifetime
  
  GvmListKeyer<ClusterVspace, ClusterKey, double> intListKeyer;
  clusters.setKeyer(&intListKeyer);
  
  for ( vector<double> & pt : listOfPoints ) {
    // Key is a list of (list of points)
    allKeys.push_back(ClusterKey ());
    ClusterKey *key = &allKeys[allKeys.size() - 1];
    key->push_back(pt);
    clusters.add(1, pt, key);
  }
  
  // There are 256 possible cluster array slots
  
  XCTAssert(clusters.clusters.size() == 256);
  
  // Only 3 clusters are actually used since there are only 3 input points
  
  vector<GvmResult<ClusterVspace, ClusterKey, double>> results = clusters.results();
  
  XCTAssert(results.size() == 3);
  
  // Reduce down to 1 cluster
  
  clusters.reduce(-1.0, 1);
  
  results = clusters.results();
  
  XCTAssert(results.size() == 1);
  
# undef ClusterKey
# undef ClusterVspace
}

// Create cluster with 3D pixel values and no custom key combiner.

- (void)testGvmClusterPair3D {
  
  // In this case, no specific key is used so padd void as key type
  
# define ClusterKey void
# define ClusterVspace GvmVectorSpace<double,3>
  
  ClusterVspace vspace;
  
  GvmClusters<ClusterVspace, ClusterKey, double> clusters(vspace, 256);
  
  // Generate list of 3D points
  
  double p1[] = { 0.0, 1.0, 2.0 };
  double p2[] = { 1.0, 1.0, 2.0 };
  double p3[] = { 2.0, 2.0, 2.0 };
  
  vector<vector<double> > listOfPoints;
  
  vector<double> points;
  
  points.clear();
  points.push_back(p1[0]);
  points.push_back(p1[1]);
  points.push_back(p1[2]);
  listOfPoints.push_back(points);
  
  points.clear();
  points.push_back(p2[0]);
  points.push_back(p2[1]);
  points.push_back(p2[2]);
  listOfPoints.push_back(points);
  
  points.clear();
  points.push_back(p3[0]);
  points.push_back(p3[1]);
  points.push_back(p3[2]);
  listOfPoints.push_back(points);
  
  // Insert each point into clusters without keys
  
  for ( vector<double> & pt : listOfPoints ) {
    clusters.add(1, pt, nullptr);
  }
  
  // There are 256 possible cluster array slots
  
  XCTAssert(clusters.clusters.size() == 256);
  
  // Only 3 clusters are actually used since there are only 3 input points
  
  vector<GvmResult<ClusterVspace, ClusterKey, double>> results = clusters.results();
  
  XCTAssert(results.size() == 3);
  
  // Reduce down to 1 cluster
  
  clusters.reduce(-1.0, 1);
  
  results = clusters.results();
  
  XCTAssert(results.size() == 1);
  
# undef ClusterKey
# undef ClusterVspace
}

/*

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

*/
 
@end
