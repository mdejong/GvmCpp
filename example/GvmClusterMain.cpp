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

#import "Gvm.hpp"

using namespace std;
using namespace Gvm;

int main(int argc, char **argv) {
//  fprintf(stdout, "hello world\n");
  
  // The cluster "key" is a list of int values that correspond
  // to a certain coordinate.
  
  typedef void ClusterKey;
  
  GvmVectorSpace<double> vspace(3);
  
  const int numClusters = 2;
  
  GvmClusters<GvmVectorSpace<double>, ClusterKey, double> clusters(vspace, numClusters);
  
//  GvmCluster<GvmVectorSpace<double>, ClusterKey, double> c1(clusters);
//  GvmCluster<GvmVectorSpace<double>, ClusterKey, double> c2(clusters);
//  GvmClusterPair<GvmVectorSpace<double>, ClusterKey, double> cPair(c1, c2);
  
  // Generate list of 3D points
  
  double p1[] = { 0.0, 0.0, 0.0 };
  double p2[] = { 10.0, 10.0, 10.0 };
  double p3[] = { 1.0, 1.0, 1.0 };
  
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
  
  for ( vector<double> & pt : listOfPoints ) {
    // Key is a list of (list of points)
    cout << "clustering point " << pt[0] << " " << pt[1] << " " << pt[2] << endl;
    clusters.add(1, pt, nullptr);
  }
  
  cout << "generated " << clusters.clusters.size() << " clusters" << endl;
  
  // vector<GvmResult>
  
  for ( auto & result : clusters.results() ) {
    cout << "cluster: " << result.toString() << endl;
  }
  
  return 0;
}
