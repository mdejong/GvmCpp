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

using namespace std;
using namespace Gvm;

@interface GvmExTenTest : XCTestCase

@end

@implementation GvmExTenTest

- (vector<vector<double> >) getTestTenSampleVec
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
  
  vector<vector<double> > coordsVec;
  
  for (int i = 0; i < sizeof(data2D)/sizeof(double); i+= 2) {
    double x = data2D[i];
    double y = data2D[i+1];
    
    vector<double> coords;
    coords.push_back(x);
    coords.push_back(y);
    
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
  
# define ClusterKey vector<vector<double> >
  
  GvmVectorSpace<double> vspace(2);
  
  GvmClusters<GvmVectorSpace<double>, ClusterKey, double> clusters(vspace, 3);
  
  vector<vector<double> > listOfPoints = [self getTestTenSampleVec];
  
  for ( vector<double> & pt : listOfPoints ) {
    clusters.add(1, pt, nullptr);
  }
  
  // 3 clusters
  
  vector<GvmResult<GvmVectorSpace<double>, ClusterKey, double>> results = clusters.results();
  
  XCTAssert(results.size() == 3);
  
# undef ClusterKey
}

@end
