GvmCpp:
A C++ impl of GVM clustering

This project is a C++ impl of GVM clustering:

http://www.tomgibara.com/clustering/fast-spatial/

The Xcode project file contains all the source needed to compile on Mac OSX with Xcode.

Open the project file in the xcode/ subdirectory to find 2 example command line programs:

PngClusterSort: Complete example that will read a PNG, dedup pixels, then cluster and sort
pixels based on clustering results.

GvmCPP: minimal pixel clustering example program showing GVM API usage.

Pass a PNG image filename into the pngclustersort command line and then have a peek at the
cluster sorted results. GVM clustering is significantly faster than other O(n^2) clustering
approaches. GVM will process all pixels from Lenna.png in about a second. A very very large
4K image consisting of 4096x2160 takes about 10 seconds. The C++ impl is about 30% faster
than the Java version.
