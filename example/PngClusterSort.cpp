//
//  PngClusterSort.cpp
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
// This example reads an input image with libpng and then filters
// duplicate pixels and sorts pixels by integer value. The sorted
// pixels are then clustered with Gvm and reordered from darker
// to lighter in terms of the 3D color cube. The output is an
// image that shows how specific pixels were clustered and
// then combined into a 2D color sorted representation where
// alike pixels are nearer to each other than they would be in
// the increasing int value order. The key aspect of the clustering
// is the it operates very quickly even for a very large image
// with lots of pixels.

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define PNG_DEBUG 3
#include "png.h"

#include <iostream>

#include <unordered_map>

#import "Gvm.hpp"

using namespace std;
using namespace Gvm;

void abort_(const char * s, ...)
{
  va_list args;
  va_start(args, s);
  vfprintf(stderr, s, args);
  fprintf(stderr, "\n");
  va_end(args);
  abort();
}

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

// PNG utils

typedef struct {
  int width, height;
  png_byte color_type;
  png_byte bit_depth;
  
  png_structp png_ptr;
  png_infop info_ptr;
  int number_of_passes;
  png_bytep * row_pointers;
  
  uint32_t *pixels;
} PngContext;

void PngContext_init(PngContext *cxt) {
  cxt->pixels = NULL;
  cxt->row_pointers = NULL;
}

// Copy setting from one PNG context to another. Note that this
// only makes a copy of the settings and not the actual pixels.
// Note that the width and height will have to be explicitly set.

void PngContext_copy_settngs(PngContext *toCxt, PngContext *fromCxt) {
  toCxt->color_type = fromCxt->color_type;
  toCxt->bit_depth = fromCxt->bit_depth;
  toCxt->png_ptr = fromCxt->png_ptr;
  toCxt->info_ptr = fromCxt->info_ptr;
  toCxt->number_of_passes = fromCxt->number_of_passes;
}

void PngContext_alloc_pixels(PngContext *cxt, int width, int height) {
  cxt->width = width;
  cxt->height = height;
  
  /* allocate pixels data and read into array of pixels */
  
  cxt->pixels = (uint32_t*) malloc(cxt->width * cxt->height * sizeof(uint32_t));
  
  if (cxt->pixels == NULL) {
    abort_("[PngContext_alloc_pixels] could not allocate %d bytes to store pixel data", (cxt->width * cxt->height * sizeof(uint32_t)));
  }
  
  memset(cxt->pixels, 0, cxt->width * cxt->height * sizeof(uint32_t));
}

// deallocate memory

void free_row_pointers(PngContext *cxt);

void PngContext_dealloc(PngContext *cxt)
{
  free_row_pointers(cxt);
  free(cxt->pixels);
}

// BGR or BGRA data will be read into this buffer of pixels

const int debugPrintPixelsReadAndWritten = 0;

void allocate_row_pointers(PngContext *cxt)
{
  if (cxt->row_pointers != NULL) {
    abort_("[allocate_row_pointers] row_pointers already allocated");
  }
  
  cxt->row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * cxt->height);
  
  if (cxt->row_pointers == NULL) {
    abort_("[allocate_row_pointers] could not allocate %d bytes to store row data", (sizeof(png_bytep) * cxt->height));
  }
  
  int y;
  
  for (y=0; y < cxt->height; y++) {
    cxt->row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(cxt->png_ptr,cxt->info_ptr));
    
    if (cxt->row_pointers[y] == NULL) {
      abort_("[allocate_row_pointers] could not allocate %d bytes to store row data", png_get_rowbytes(cxt->png_ptr,cxt->info_ptr));
    }
  }
}

void free_row_pointers(PngContext *cxt)
{
  if (cxt->row_pointers == NULL) {
    return;
  }
  
  int y;
  
  for (y=0; y < cxt->height; y++) {
    free(cxt->row_pointers[y]);
  }
  free(cxt->row_pointers);
  cxt->row_pointers = NULL;
}

void read_png_file(char* file_name, PngContext *cxt)
{
  char header[8];    // 8 is the maximum size that can be checked
  
  PngContext_init(cxt);
  
  /* open file and test for it being a png */
  FILE *fp = fopen(file_name, "rb");
  if (!fp)
    abort_("[read_png_file] File %s could not be opened for reading", file_name);
  fread(header, 1, 8, fp);
  if (png_sig_cmp((png_const_bytep)header, 0, 8))
    abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);
  
  /* initialize stuff */
  cxt->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  
  if (cxt->png_ptr == NULL)
    abort_("[read_png_file] png_create_read_struct failed");
  
  cxt->info_ptr = png_create_info_struct(cxt->png_ptr);
  if (cxt->info_ptr == NULL)
    abort_("[read_png_file] png_create_info_struct failed");
  
  if (setjmp(png_jmpbuf(cxt->png_ptr)))
    abort_("[read_png_file] Error during init_io");
  
  png_init_io(cxt->png_ptr, fp);
  png_set_sig_bytes(cxt->png_ptr, 8);
  
  png_read_info(cxt->png_ptr, cxt->info_ptr);
  
  int width = png_get_image_width(cxt->png_ptr, cxt->info_ptr);
  int height = png_get_image_height(cxt->png_ptr, cxt->info_ptr);
  
  PngContext_alloc_pixels(cxt, width, height);
  
  cxt->color_type = png_get_color_type(cxt->png_ptr, cxt->info_ptr);
  cxt->bit_depth = png_get_bit_depth(cxt->png_ptr, cxt->info_ptr);
  
  cxt->number_of_passes = png_set_interlace_handling(cxt->png_ptr);
  png_read_update_info(cxt->png_ptr, cxt->info_ptr);
  
  if (cxt->bit_depth != 8) {
    abort_("[read_png_file] only 8 bit pixel depth PNG is supported");
  }
  
  /* read file */
  if (setjmp(png_jmpbuf(cxt->png_ptr)))
    abort_("[read_png_file] Error during read_image");
  
  allocate_row_pointers(cxt);
  
  png_read_image(cxt->png_ptr, cxt->row_pointers);
  
  int pixeli = 0;
  
  int isBGRA = 0;
  int isGrayscale = 0;
  
  png_byte ctByte = png_get_color_type(cxt->png_ptr, cxt->info_ptr);
  
  if (ctByte == PNG_COLOR_TYPE_RGBA) {
    isBGRA = 1;
    
    if ((1)) {
      fprintf(stdout, "input contains alpha channel, only opaque input images supported\n");
      exit(1);
    }
    
  } else if (ctByte == PNG_COLOR_TYPE_RGB) {
    isBGRA = 0;
  } else if (ctByte == PNG_COLOR_TYPE_GRAY) {
    isGrayscale = 1;
  } else {
    abort_("[read_png_file] unsupported input format type");
  }
  
  for (int y=0; y < cxt->height; y++) {
    png_byte* row = cxt->row_pointers[y];
    
    if (isGrayscale) {
      for (int x=0; x < cxt->width; x++) {
        png_byte* ptr = &(row[x]);
        
        uint32_t gray = ptr[0];
        uint32_t pixel = (0xFF << 24) | (gray << 16) | (gray << 8) | gray;
        
        if (debugPrintPixelsReadAndWritten) {
          fprintf(stdout, "Read pixel 0x%08X at (x,y) (%d, %d)\n", pixel, x, y);
        }
        
        cxt->pixels[pixeli] = pixel;
        
        pixeli++;
      }
    } else if (isBGRA) {
      for (int x=0; x < cxt->width; x++) {
        png_byte* ptr = &(row[x*4]);
        
        uint32_t B = ptr[2];
        uint32_t G = ptr[1];
        uint32_t R = ptr[0];
        uint32_t A = ptr[3];
        
        uint32_t pixel = (A << 24) | (R << 16) | (G << 8) | B;
        
        if (debugPrintPixelsReadAndWritten) {
          fprintf(stdout, "Read pixel 0x%08X at (x,y) (%d, %d)\n", pixel, x, y);
        }
        
        cxt->pixels[pixeli] = pixel;
        
        pixeli++;
      }
    } else {
      // BGR with no alpha channel
      for (int x=0; x < cxt->width; x++) {
        png_byte* ptr = &(row[x*3]);
        
        uint32_t B = ptr[2];
        uint32_t G = ptr[1];
        uint32_t R = ptr[0];
        
        uint32_t pixel = (0xFF << 24) | (R << 16) | (G << 8) | B;
        
        if (debugPrintPixelsReadAndWritten) {
          fprintf(stdout, "Read pixel 0x%08X at (x,y) (%d, %d)\n", pixel, x, y);
        }
        
        cxt->pixels[pixeli] = pixel;
        
        pixeli++;
      }
    }
  }
  
  fclose(fp);
  
  free_row_pointers(cxt);
}

double clampByte(double d) {
  if (d < 0.0) {
    d = 0.0;
  } else if (d > 255.0) {
    d = 255.0;
  }
  return d;
}

// Given a vector of pixels and a pixel that may or may not be in the vector, return
// the pixel in the vector that is closest to the indicated pixel.

uint32_t closestToPixel(const vector<uint32_t> &pixels, const uint32_t closeToPixel) {
  const bool debug = false;
  
#if defined(DEBUG)
  assert(pixels.size() > 0);
#endif // DEBUG
  
  unsigned int minDist = (~0);
  uint32_t closestToPixel = 0;
  
  uint32_t cB = closeToPixel & 0xFF;
  uint32_t cG = (closeToPixel >> 8) & 0xFF;
  uint32_t cR = (closeToPixel >> 16) & 0xFF;
  
  for ( uint32_t pixel : pixels ) {
    uint32_t B = pixel & 0xFF;
    uint32_t G = (pixel >> 8) & 0xFF;
    uint32_t R = (pixel >> 16) & 0xFF;
    
    int dB = (int)cB - (int)B;
    int dG = (int)cG - (int)G;
    int dR = (int)cR - (int)R;
    
    unsigned int d3 = (unsigned int) ((dB * dB) + (dG * dG) + (dR * dR));
    
    if (debug) {
      cout << "d3 from (" << B << "," << G << "," << R << ") to closeToPixel (" << cB << "," << cG << "," << cR << ") is (" << dB << "," << dG << "," << dR << ") = " << d3 << endl;
    }
    
    if (d3 < minDist) {
      closestToPixel = pixel;
      minDist = d3;
      
      if ((debug)) {
        fprintf(stdout, "new    closestToPixel 0x%08X\n", closestToPixel);
      }
      
      if (minDist == 0) {
        // Quit the loop once a zero distance has been found
        break;
      }
    }
  }
  
  if ((debug)) {
    fprintf(stdout, "return closestToPixel 0x%08X\n", closestToPixel);
  }
  
  return closestToPixel;
}

// Iterate over clusters and determine the cluster center pixel

template<typename K,typename R>
vector<uint32_t> get_cluster_centers(R &resultVec) {
  vector<uint32_t> clusterCenterPixels;
  
  // In DEBUG mode, do extra checking to make sure that no 2 clusters contain the same pixel
  // to prevent a possible case where the closest to cluster center pixel for two different
  // clusters is the same pixel.
  
#if defined(DEBUG)
  unordered_map<uint32_t, uint32_t> allPixelsClusterOffset;
#endif // DEBUG
  
  uint32_t clusteri = 0;
  
  for (int i = 0; i < resultVec.size(); i++) {
    auto &result = resultVec[i];
    K *cluster = result.getKey();
  
#if defined(DEBUG)
    for ( uint32_t pixel : *cluster ) {
      assert(allPixelsClusterOffset.count(pixel) == 0);
      allPixelsClusterOffset[pixel] = clusteri;
    }
#endif // DEBUG

    double Bd = round(result.point[0]);
    double Gd = round(result.point[1]);
    double Rd = round(result.point[2]);
    
    Bd = clampByte(Bd);
    Gd = clampByte(Gd);
    Rd = clampByte(Rd);
    
    uint32_t Bb = Bd;
    uint32_t Gb = Gd;
    uint32_t Rb = Rd;
    
    uint32_t comPixel = (Rb << 16) | (Gb << 8) | Bb;
    uint32_t closestToCenterOfMassPixel = closestToPixel(*cluster, comPixel);
    
#if defined(DEBUG)
    // The center of mass pixel should already have been seen above
    assert(allPixelsClusterOffset.count(closestToCenterOfMassPixel) > 0);
    assert(allPixelsClusterOffset[closestToCenterOfMassPixel] == clusteri);
#endif // DEBUG
    
    // Force 24BPP cluster centers
    
    clusterCenterPixels.push_back(closestToCenterOfMassPixel & 0x00FFFFFF);
    clusteri += 1;
  }
  
  return clusterCenterPixels;
}

// Given a vector of cluster center pixels, determine a cluster to cluster walk order based on 3D
// distance from one cluster center to the next. This method returns a vector of offsets into
// the cluster table with the assumption that the number of clusters fits into a 16 bit offset.

vector<uint32_t> generate_cluster_walk_on_center_dist(const vector<uint32_t> &clusterCenterPixels)
{
  const bool debugDumpClusterWalk = false;
  
  int numClusters = (int) clusterCenterPixels.size();
  
  unordered_map<uint32_t, uint32_t> clusterCenterToClusterOffsetMap;
  
  int clusteri = 0;
  
  for ( clusteri = 0; clusteri < numClusters; clusteri++) {
    uint32_t closestToCenterOfMassPixel = clusterCenterPixels[clusteri];
    clusterCenterToClusterOffsetMap[closestToCenterOfMassPixel] = clusteri;
  }
  
  // Reorder the clusters so that the first cluster contains the pixel with the value
  // that is closest to zero. Then, the next cluster is determined by checking
  // the distance to the first pixel in the next cluster. The clusters are already
  // ordered in terms of density so this reordering logic basically jumps from one
  // cluster to the next in terms of the shortest distance to the next clsuter.
  
  vector<uint32_t> closestSortedClusterOrder;
  
  closestSortedClusterOrder.reserve(numClusters);
  
  if ((1)) {
    // Choose cluster that is closest to (0,0,0)
    
    uint32_t closestToZeroCenter = closestToPixel(clusterCenterPixels, 0x0);
    
    int closestToZeroClusteri = -1;
    
    clusteri = 0;
    
    for ( uint32_t clusterCenter : clusterCenterPixels ) {
      if (closestToZeroCenter == clusterCenter) {
        closestToZeroClusteri = clusteri;
        break;
      }
      
      clusteri += 1;
    }
    
    assert(closestToZeroClusteri != -1);
    
    if (debugDumpClusterWalk) {
      fprintf(stdout, "closestToZero 0x%08X is in clusteri %d\n", closestToZeroCenter, closestToZeroClusteri);
    }
    
    closestSortedClusterOrder.push_back(closestToZeroClusteri);
    
    // Calculate the distance from the cluster center to the next closest cluster center.
    
    {
      uint32_t closestToCenterOfMassPixel = clusterCenterPixels[closestToZeroClusteri];
      
      auto it = clusterCenterToClusterOffsetMap.find(closestToCenterOfMassPixel);
      
#if defined(DEBUG)
      assert(it != end(clusterCenterToClusterOffsetMap));
#endif // DEBUG
      
      clusterCenterToClusterOffsetMap.erase(it);
    }
    
    // Each remaining cluster is represented by an entry in clusterCenterToClusterOffsetMap.
    // Get the center coord and use the center to lookup the cluster index. Then find
    // the next closest cluster in terms of distance in 3D space.
    
    uint32_t closestCenterPixel = clusterCenterPixels[closestToZeroClusteri];
    
    for ( ; 1 ; ) {
      if (clusterCenterToClusterOffsetMap.size() == 0) {
        break;
      }
      
      vector<uint32_t> remainingClustersCenterPoints;
      
      for ( auto it = begin(clusterCenterToClusterOffsetMap); it != end(clusterCenterToClusterOffsetMap); it++) {
        //uint32_t clusterCenterPixel = it->first;
        uint32_t clusterOffset = it->second;
        
        uint32_t clusterStartPixel = clusterCenterPixels[clusterOffset];
        remainingClustersCenterPoints.push_back(clusterStartPixel);
      }
      
      if (debugDumpClusterWalk) {
        fprintf(stdout, "there are %d remaining center pixel clusters\n", (int)remainingClustersCenterPoints.size());
        
        for ( uint32_t pixel : remainingClustersCenterPoints ) {
          fprintf(stdout, "0x%08X\n", pixel);
        }
      }
      
      uint32_t nextClosestClusterCenterPixel = closestToPixel(remainingClustersCenterPoints, closestCenterPixel);
      
      if (debugDumpClusterWalk) {
        fprintf(stdout, "nextClosestClusterPixel is 0x%08X from current clusterEndPixel 0x%08X\n", nextClosestClusterCenterPixel, closestCenterPixel);
      }
      
      assert(nextClosestClusterCenterPixel != closestCenterPixel);
      
#if defined(DEBUG)
      assert(clusterCenterToClusterOffsetMap.count(nextClosestClusterCenterPixel) > 0);
#endif // DEBUG
      
      uint32_t nextClosestClusteri = clusterCenterToClusterOffsetMap[nextClosestClusterCenterPixel];
      
      closestSortedClusterOrder.push_back(nextClosestClusteri);
      
      {
        // Find index from next closest and then lookup cluster center
        
        uint32_t nextClosestCenterPixel = clusterCenterPixels[nextClosestClusteri];
        
        auto it = clusterCenterToClusterOffsetMap.find(nextClosestCenterPixel);
#if defined(DEBUG)
        assert(it != end(clusterCenterToClusterOffsetMap));
#endif // DEBUG
        clusterCenterToClusterOffsetMap.erase(it);
      }
      
      closestCenterPixel = nextClosestClusterCenterPixel;
    }
    
    assert(closestSortedClusterOrder.size() == clusterCenterPixels.size());
  }
  
  return closestSortedClusterOrder;
}

void write_png_file(char* file_name, PngContext *cxt)
{
  /* create file */
  FILE *fp = fopen(file_name, "wb");
  if (!fp)
    abort_("[write_png_file] File %s could not be opened for writing", file_name);
  
  
  /* initialize stuff */
  cxt->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  
  if (cxt->png_ptr == NULL)
    abort_("[write_png_file] png_create_write_struct failed");
  
  cxt->info_ptr = png_create_info_struct(cxt->png_ptr);
  if (cxt->info_ptr == NULL)
    abort_("[write_png_file] png_create_info_struct failed");
  
  if (setjmp(png_jmpbuf(cxt->png_ptr)))
    abort_("[write_png_file] Error during init_io");
  
  png_init_io(cxt->png_ptr, fp);
  
  
  /* write header */
  if (setjmp(png_jmpbuf(cxt->png_ptr)))
    abort_("[write_png_file] Error during writing header");
  
  png_set_IHDR(cxt->png_ptr, cxt->info_ptr, cxt->width, cxt->height,
               cxt->bit_depth, cxt->color_type, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
  
  png_write_info(cxt->png_ptr, cxt->info_ptr);
  
  /* write pixels back to row_pointers */
  
  allocate_row_pointers(cxt);
  
  int isBGRA = 0;
  int isGrayscale = 0;
  
  png_byte ctByte = png_get_color_type(cxt->png_ptr, cxt->info_ptr);
  
  if (ctByte == PNG_COLOR_TYPE_RGBA) {
    isBGRA = 1;
  } else if (ctByte == PNG_COLOR_TYPE_RGB) {
    isBGRA = 0;
  } else if (ctByte == PNG_COLOR_TYPE_GRAY) {
    isGrayscale = 1;
  } else {
    abort_("[write_png_file] unsupported input format type");
  }
  
  int pixeli = 0;
  
  for (int y=0; y < cxt->height; y++) {
    png_byte* row = cxt->row_pointers[y];
    
    if (isGrayscale) {
      for (int x=0; x < cxt->width; x++) {
        png_byte* ptr = &(row[x]);
        
        uint32_t pixel = cxt->pixels[pixeli];
        
        uint32_t gray = pixel & 0xFF;
        
        ptr[0] = gray;
        
        if (debugPrintPixelsReadAndWritten) {
          fprintf(stdout, "Wrote pixel 0x%08X at (x,y) (%d, %d)\n", pixel, x, y);
        }
        
        pixeli++;
      }
    } else if (isBGRA) {
      for (int x=0; x < cxt->width; x++) {
        png_byte* ptr = &(row[x*4]);
        
        uint32_t pixel = cxt->pixels[pixeli];
        
        uint32_t B = pixel & 0xFF;
        uint32_t G = (pixel >> 8) & 0xFF;
        uint32_t R = (pixel >> 16) & 0xFF;
        uint32_t A = (pixel >> 24) & 0xFF;
        
        ptr[0] = R;
        ptr[1] = G;
        ptr[2] = B;
        ptr[3] = A;
        
        if (debugPrintPixelsReadAndWritten) {
          fprintf(stdout, "Wrote pixel 0x%08X at (x,y) (%d, %d)\n", pixel, x, y);
        }
        
        pixeli++;
      }
    } else {
      for (int x=0; x < cxt->width; x++) {
        png_byte* ptr = &(row[x*3]);
        
        uint32_t pixel = cxt->pixels[pixeli];
        
        uint32_t B = pixel & 0xFF;
        uint32_t G = (pixel >> 8) & 0xFF;
        uint32_t R = (pixel >> 16) & 0xFF;
        
        ptr[0] = R;
        ptr[1] = G;
        ptr[2] = B;
        
        if (debugPrintPixelsReadAndWritten) {
          fprintf(stdout, "Wrote pixel 0x%08X at (x,y) (%d, %d)\n", pixel, x, y);
        }
        
        pixeli++;
      }
    }
  }
  
  if (setjmp(png_jmpbuf(cxt->png_ptr)))
    abort_("[write_png_file] Error during writing bytes");
  
  png_write_image(cxt->png_ptr, cxt->row_pointers);
  
  
  /* end write */
  if (setjmp(png_jmpbuf(cxt->png_ptr)))
    abort_("[write_png_file] Error during end of write");
  
  png_write_end(cxt->png_ptr, NULL);
  
  fclose(fp);
}

void process_file(PngContext *cxt)
{
  // Input contains all pixels from image, dedup pixels using
  // an unordered_map and then sort after the dedup.
  
  unordered_map<uint32_t, uint32_t> uniquePixelMap;
  
  int numPixels = cxt->width * cxt->height;
  
  printf("read  %d pixels from input image\n", numPixels);
  
  for ( int i = 0; i < numPixels; i++) {
    uint32_t pixel = cxt->pixels[i];
    uniquePixelMap[pixel] = 0;
  }
  
  vector<uint32_t> allPixels;
  
  for ( auto it = begin(uniquePixelMap); it != end(uniquePixelMap); it++ ) {
    uint32_t pixel = it->first;
    allPixels.push_back(pixel);
  }
  
  // Release possibly large amount of memory used for hashtable
  uniquePixelMap = unordered_map<uint32_t, uint32_t>();
  
  // Sort pixels into int order (this can take some time for large N)
  
  sort(begin(allPixels), end(allPixels));
  
  // Using float instead of double cuts memory usage down just a bit, like 10%
  
  typedef double FP;
  //  typedef float FP;
  
  // ClusterVector and ClusterVspace define the low level fixed size
  // vector of values that represents the point values.
  
  typedef GvmStdVector<FP,3> ClusterVector;
  typedef GvmVectorSpace<ClusterVector,FP,3> ClusterVectorSpace;
  
  // A "key" is a vector of points in one specific cluster.
  
  typedef vector<uint32_t> ClusterKey;
  
#if defined(DEBUG)
  checkForDuplicates(allPixels, 0);
#endif // DEBUG
  
  printf("found %d unique pixels in input image\n", (int)allPixels.size());
  
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
  
  printf("generated %d clusters\n", (int)clusters.getCapacity());
  
  // Emit clustered results as YUV data padded out to 256 with zeros
  
  // vector<GvmResult>
  
  int clusteri = 0;
  
  vector<GvmResult<ClusterVectorSpace, ClusterVector, ClusterKey, FP>> results = clusters.results();
  
  for ( auto & result : results ) {
    printf("cluster[%d]: %s\n", clusteri, result.toString().c_str());
    clusteri++;
  }
  
  // Write clustered pixels and PNG image with N pixels in rows of at least 256
  
  int totalPixelsWritten = 0;
  
  // Write image that contains pixels clustered into N clusters where each row in
  // the image corresponds to a cluster
  
  PngContext cxt2;
  PngContext_init(&cxt2);
  PngContext_copy_settngs(&cxt2, cxt);
  
  // Count num rows needed to represent pixels with max width 256
  
  const int numCols = 256;
  int numRows = 0;
  
  {
    for (int i = 0; i < results.size(); i++) {
      ClusterKey *clusterKeys = results[i].getKey();
      
      int N = (int) clusterKeys->size();
      
      printf("cluster[%3d]: contains %5d pixels\n", i, N);
      
      if (N == 0) {
        // Emit empty row in this case
        numRows += 1;
      } else if (N < numCols) {
        numRows += 1;
      } else {
        while (N > 0) {
          numRows++;
          if (N == numCols) {
            numRows++;
          }
          N -= numCols;
        }
      }
    }
  }
  
  // Allocate columns x height pixels
  
  PngContext_alloc_pixels(&cxt2, numCols, numRows);
  
  uint32_t *outPixels = cxt2.pixels;
  uint32_t outPixelsi = 0;
  
  for (int i = 0; i < results.size(); i++) {
    ClusterKey *clusterKeys = results[i].getKey();
    
    int pixelsWritten = 0;
    
    for ( uint32_t pixel : *clusterKeys ) {
      outPixels[outPixelsi++] = pixel;
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
        outPixels[outPixelsi++] = pixel;
        totalPixelsWritten++;
      }
    }
  }
  
  char *outClustersFilename = (char*)"clusters.png";
  
  write_png_file((char*)outClustersFilename, &cxt2);
  
  printf("wrote %d cluster pixels to %s\n", totalPixelsWritten, outClustersFilename);
  
  int totalRowsOf256 = totalPixelsWritten/256;
  if ((totalPixelsWritten % 256) != 0) {
    assert(0);
  }
  assert(totalRowsOf256 == numRows);

  PngContext_dealloc(&cxt2);
  
  // Resort cluster in terms of shortest distance from one cluster center to the next
  // to implement a cluster sort order.
  
  vector<uint32_t> clusterCenterPixels = get_cluster_centers<ClusterKey, vector<GvmResult<ClusterVectorSpace, ClusterVector, ClusterKey, FP>>>(results);
  
//  for ( uint32_t pixel : clusterCenterPixels ) {
//    printf("center pixel 0x%08X\n", pixel);
//  }

  // Generate cluster to cluster walk (sort) order
  
  vector<uint32_t> sortedOffset = generate_cluster_walk_on_center_dist(clusterCenterPixels);
  
  // Combine pixels into a flat array of pixels and emit as image
  // with 256 columns.
  
  // Combine all cluster rows into a flat array of pixels in color group sorted order
  
  PngContext cxt3;
  PngContext_init(&cxt3);
  PngContext_copy_settngs(&cxt3, cxt);
  
  allPixels.clear();
  
  for (int i = 0; i < results.size(); i++) {
    uint32_t si = sortedOffset[i];
    ClusterKey *clusterKeys = results[si].getKey();
    
    for ( uint32_t pixel : *clusterKeys ) {
      allPixels.push_back(pixel);
    }
  }
  
#if defined(DEBUG)
  checkForDuplicates(allPixels, 0);
#endif // DEBUG

  numRows = (int)allPixels.size() / 256;
  if (((int)allPixels.size() % 256) != 0) {
    numRows++;
  }
  
  PngContext_alloc_pixels(&cxt3, 256, numRows);
  
  int pixeli = 0;
  uint32_t *outPixelsPtr = (uint32_t*)cxt3.pixels;
  
  for ( uint32_t pixel : allPixels ) {
    outPixelsPtr[pixeli++] = pixel;
  }
  
  char *outSortedClustersFilename = (char*)"sorted.png";
  
  write_png_file(outSortedClustersFilename, &cxt3);
  
  printf("wrote %d total sorted pixels to %s\n", (int)allPixels.size(), outSortedClustersFilename);
  
  PngContext_dealloc(&cxt3);
  
  return;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "usage pngclustersort PNG\n");
    exit(1);
  }
  
  PngContext cxt;
  read_png_file(argv[1], &cxt);
  
  printf("processing %d pixels from image of dimensions %d x %d\n", cxt.width*cxt.height, cxt.width, cxt.height);
  
  process_file(&cxt);
  
  PngContext_dealloc(&cxt);

}
