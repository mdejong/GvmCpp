//
//  GvmMouseTest.mm
//  GvmTest
//
//  Created by Mo DeJong on 8/4/15.
//  Copyright (c) 2015 helpurock. All rights reserved.
//
//  Cluster the "gmouse.txt" 2D test dataset from Gvm distro.

#import <Cocoa/Cocoa.h>
#import <XCTest/XCTest.h>

#import "Gvm.hpp"

#include <iostream>

using namespace std;
using namespace Gvm;

@interface GvmMouseTest : XCTestCase

@end

@implementation GvmMouseTest

- (vector<vector<double> >) getTestSampleVec
{
  double data2D[] = {
    0.33,-4.561,
    2.549,1.528,
    -1.082,5.366,
    4.986,-3.766,
    -1.829,-4.193,
    -0.23,-1.158,
    -3.812,1.046,
    -1.403,3.59,
    8.626,-0.991,
    -4.748,5.274,
    -1.008,-2.723,
    -0.044,-2.564,
    2.86,-6.001,
    -0.229,9.552,
    1.276,1.18,
    1.381,6.159,
    1.077,-3.632,
    -3.272,2.095,
    0.694,-11.902,
    -2.482,-1.514,
    -0.33,-6.739,
    0.271,-2.934,
    2.301,7.573,
    0.299,1.696,
    2.666,3.481,
    7.635,0.292,
    -2.355,-3.754,
    -8.143,-1.403,
    3.755,3.167,
    -2.349,-1.505,
    -3.157,2.908,
    -4.675,-1.139,
    4.021,9.102,
    -0.808,4.438,
    -0.802,4.909,
    6.908,-1.045,
    1.781,-6.235,
    3.551,-4.225,
    8.191,0.029,
    -4.145,-5.68,
    3.244,4.369,
    -2.495,-2.715,
    5.879,0.343,
    1.794,-12.291,
    -2.089,2.58,
    2.898,4.623,
    -0.812,4.948,
    5.42,-2.331,
    -6.689,4.798,
    -3.257,0.545,
    1.597,-3.35,
    -2.82,3.252,
    -1.373,5.56,
    -6.845,-3.487,
    -4.104,4.605,
    -1.009,1.296,
    -0.345,-1.376,
    -0.118,-0.28,
    -2.455,1.43,
    -9.745,-7.146,
    -8.374,1.265,
    1.44,-2.456,
    -4.5,-5.898,
    -4.473,2.569,
    6.42,0.14,
    6.62,2.416,
    4.18,-3.488,
    3.636,3.184,
    1.54,-1.552,
    2.246,4.03,
    2.8,-3.59,
    7.398,-0.404,
    -4.837,2.219,
    1.36,2.185,
    -6.508,2.979,
    5.11,1.152,
    2.454,1.552,
    5.023,3.576,
    1.954,3.765,
    -2.79,0.203,
    1.546,4.602,
    -8.76,-3.844,
    -0.142,1.688,
    0.306,-0.543,
    -9.266,5.187,
    2.501,-1.418,
    -2.731,-1.003,
    -1.008,5.815,
    1.878,1.571,
    -4.048,2.603,
    3.46,4.465,
    -6.555,-4.517,
    1.996,0.508,
    -0.617,0.693,
    -2.452,-2.925,
    -2.34,4.826,
    3.772,0.622,
    -3.823,3.495,
    -4.888,-1.837,
    -0.747,-4.698,
    -4.035,3.999,
    -4.683,6.336,
    -7.397,4.057,
    -5.916,3.561,
    -5.742,3.506,
    -4.62,4.677,
    -3.546,3.634,
    -4.338,2.203,
    -7.427,3.121,
    -2.973,6.234,
    -3.767,6.128,
    -4.214,2.044,
    -1.431,5.156,
    -6.5,1.249,
    -2.523,5.004,
    -5.425,2.573,
    -4.603,4.967,
    -6.911,-0.137,
    -6.208,6.571,
    -3.265,4.069,
    0.143,4.793,
    -4.086,3.905,
    -2.59,3.972,
    -0.899,4.638,
    -4.229,2.209,
    -5.349,-1.633,
    -4.133,4.669,
    -6.345,5.125,
    -5.5,8.085,
    -5.762,10.094,
    -0.31,5.561,
    -0.949,5.145,
    -6.562,4.684,
    0.088,3.687,
    -4.29,4.805,
    -4.044,3.726,
    -4.381,2.035,
    -3.868,4.894,
    -6.907,7.1,
    -3.657,2.906,
    -3.111,1.146,
    -4.633,7.998,
    -3.883,2.79,
    -7.025,3.863,
    -4.698,2.7,
    -7.022,8.482,
    -5.936,2.683,
    -5.876,5.535,
    -3.832,3.604,
    -3.825,1.363,
    4.234,2.628,
    5.005,0.343,
    2.91,3.475,
    1.4,4.884,
    3.199,7.089,
    2.616,3.831,
    3.376,3.69, 
    4.435,4.751, 
    1.844,5.398, 
    6.852,3.846, 
    0.06,1.547, 
    2.267,3.694, 
    2.815,0.763, 
    4.05,0.982, 
    2.104,3.566, 
    5.905,3.924, 
    3.486,6.517, 
    4.003,4.991, 
    3.779,4.817, 
    3.739,5.782, 
    3.853,3.899, 
    5.053,1.612, 
    1.279,1.715, 
    4.135,3.09, 
    0.928,6.985, 
    6.051,3.657, 
    2.524,2.941, 
    5.527,4.439, 
    2.24,6.746, 
    1.561,0.72, 
    1.678,2.077, 
    5.74,4.952, 
    2.178,4.756, 
    1.897,2.954, 
    5.778,5.939, 
    3.012,2.961, 
    4.044,4.121, 
    3.715,4.725, 
    6.927,3.489, 
    4.775,2.01, 
    3.315,3.304, 
    2.117,2.564, 
    4.608,2.816, 
    7.495,1.17, 
    5.505,3.996, 
    1.023,6.015, 
    5.23,6.502, 
    -3.752,4.57, 
    -0.033,7.249, 
    1.898,4.452
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
  
  assert(coordsVec.size() == 200);
  
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

- (void)testGvmMouse {

  // The cluster "key" is a list of int values that correspond
  // to a certain coordinate.
  
# define ClusterKey vector<vector<double> >
# define ClusterVspace GvmVectorSpace<double,2>
  
  // 2D data clustered into 3 clusters
  
  ClusterVspace vspace;
  
  GvmClusters<ClusterVspace, ClusterKey, double> clusters(vspace, 3);
  
  vector<vector<double> > listOfPoints = [self getTestSampleVec];
  
  for ( vector<double> & pt : listOfPoints ) {
    clusters.add(1, pt, nullptr);
  }
  
  // 3 clusters
  
  vector<GvmResult<ClusterVspace, ClusterKey, double>> results = clusters.results();
  
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
  
  XCTAssert((result0.count + result1.count + result2.count) == 200);
  
  // cluster 0: count 82
  
  XCTAssert(result0.count == 82);
  
  // variance: 16.226, mass 82
  
  XCTAssert(result0.getMass() == 82.000);
  XCTAssert(int(round(result0.getVariance() * 10.0)) == 162);
  
  // cluster center of mass
  // [3.1062073170731703, 1.0145365853658537]
  cx = result0.point[0];
  cy = result0.point[1];
  XCTAssert(int(round(cx * 100.0)) == 311);
  XCTAssert(int(round(cy * 100.0)) == 101);
  
  // cluster 1: count 14
  // getVariance: 22.953492290816307, mass: 14.0
  
  XCTAssert(result1.count == 14);
  XCTAssert(result1.getMass() == 14);
  XCTAssert(int(round(result1.getVariance() * 100.0)) == 2295);
  
  // cluster center of mass
  // [-3.574785714285714, -5.630857142857143]
  cx = result1.point[0];
  cy = result1.point[1];
  XCTAssert(int(round(cx * 100.0)) == -357);
  XCTAssert(int(round(cy * 100.0)) == -563);
  
  // cluster 2: count 104
  // getVariance: 15.006316329696741 mass: 104.0

  XCTAssert(result2.count == 104);
  
  XCTAssert(result2.getMass() == 104);
  XCTAssert(int(round(result2.getVariance() * 10.0)) == 150);
  
  // cluster center of mass
  // [-2.611201923076924, 4.1185673076923095]
  cx = result2.point[0];
  cy = result2.point[1];
  XCTAssert(int(round(cx * 10.0)) == -26);
  XCTAssert(int(round(cy * 10.0)) == 41);
  
# undef ClusterKey
# undef ClusterVspace
}

@end
