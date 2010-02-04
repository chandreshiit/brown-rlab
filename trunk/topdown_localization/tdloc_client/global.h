/**
 * Brown University, Computer Science Department
 *
 * Author: Jonas Schwertfeger <js at cs.brown.edu>
 * Date:   9/20/2007
 *
 */

#ifndef GLOBAL_H
#define GLOBAL_H GLOBAL_H

#include <sstream>
#include <iostream>
#include <vector>
#include "./include/CImg.h"

#ifndef NULL
#define NULL (void*)0
#endif

//-----------------------------------------------------------------------------------------------------------
template<class T>
bool from_string(T& t, const std::string& s, std::ios_base& (*f)(std::ios_base&)) {
  std::istringstream iss(s);
  return !(iss >> f >> t).fail();
}

//-----------------------------------------------------------------------------------------------------------
typedef struct Blob_ {
  int x1;
  int y1;
  int x2;
  int y2;
  int color;
} Blob;

//-----------------------------------------------------------------------------------------------------------
typedef unsigned char Pixel;
typedef cimg_library::CImg<Pixel> Image;

#endif // GLOBAL_H
