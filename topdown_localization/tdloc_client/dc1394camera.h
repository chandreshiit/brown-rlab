/**
 * Brown University, Computer Science Department
 *
 * Author: Jonas Schwertfeger <js at cs.brown.edu>
 * Date:   9/20/2007
 *
 */

#ifndef DC1394_CAMERA_H
#define DC1394_CAMERA_H DC1394_CAMERA_H

#include "./global.h"
#include "./include/CImg.h"
#include <libraw1394/raw1394.h>
#include <libdc1394/dc1394_control.h>

namespace rlab {

class DC1394Camera
{
public:
  enum VideoMode {
    VIDEOMODE_320x240_YUV422,
    VIDEOMODE_640x480_YUV422,
    VIDEOMODE_640x480_RGB
  };

public:
  DC1394Camera(int portNum, int camNum, VideoMode mode);
  ~DC1394Camera();

  bool startCapturing();
  bool stopCapturing();

  bool getFrame(cimg_library::CImg<unsigned char> &frame);
  bool getOneShotFrame(cimg_library::CImg<unsigned char> &frame);

  bool setExposureValue(unsigned int value) const;

private:
  void setUp();
  void tearDown();

  void storeFrame(cimg_library::CImg<unsigned char> &frame);

private:
  raw1394handle_t raw1394Handle_;
  dc1394_cameracapture cam_;
  int portNum_;
  int camNum_;
  VideoMode mode_;
  bool capturing_;

};

} // namespace rlab

#endif // DC1394_CAMERA_H
