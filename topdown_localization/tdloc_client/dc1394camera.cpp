/**
 * Brown University, Computer Science Department
 *
 * Author: Jonas Schwertfeger <js at cs.brown.edu>
 * Date:   9/20/2007
 *
 */

#include "./dc1394camera.h"
#include <stdexcept>
#include <sstream>
#include <iostream>

namespace rlab {

using namespace std;
using namespace cimg_library;

//-----------------------------------------------------------------------------------------------------------
DC1394Camera::DC1394Camera(int portNum, int camNum, VideoMode mode)
  : raw1394Handle_(NULL),
    portNum_(portNum),
    camNum_(camNum),
    mode_(mode),
    capturing_(false)
{
  setUp();
}


//-----------------------------------------------------------------------------------------------------------
DC1394Camera::~DC1394Camera()
{
  tearDown();
}


//-----------------------------------------------------------------------------------------------------------
bool DC1394Camera::startCapturing()
{
  if (capturing_) {
    return true;
  }

  if (dc1394_start_iso_transmission(raw1394Handle_, cam_.node) == DC1394_SUCCESS) {
    capturing_ = true;
    return true;
  }
  else {
    return false;
  }
}


//-----------------------------------------------------------------------------------------------------------
bool DC1394Camera::stopCapturing()
{
  if (!capturing_) {
    return true;
  }

  capturing_ = false;
  if (dc1394_stop_iso_transmission(raw1394Handle_, cam_.node)  == DC1394_SUCCESS) {
    dc1394_camera_off(raw1394Handle_, cam_.node);
    return true;
  }
  else {
    dc1394_camera_off(raw1394Handle_, cam_.node);
    return false;
  }
}


//-----------------------------------------------------------------------------------------------------------
bool DC1394Camera::getFrame(CImg<unsigned char> &frame)
{
  if (!capturing_ && !startCapturing()) {
    return false;
  }

  if (dc1394_dma_single_capture(&cam_) != DC1394_SUCCESS) {
    return false;
  }

  storeFrame(frame);

  dc1394_dma_done_with_buffer(&cam_);

  return true;
}


//-----------------------------------------------------------------------------------------------------------
bool DC1394Camera::getOneShotFrame(CImg<unsigned char> &frame)
{
  if (capturing_ && !stopCapturing()) {
    return false;
  }

  if (dc1394_set_one_shot(raw1394Handle_, cam_.node) != DC1394_SUCCESS) {
    return false;
  }

  if (dc1394_dma_single_capture(&cam_) != DC1394_SUCCESS) {
    return false;
  }

  storeFrame(frame);

  dc1394_dma_done_with_buffer(&cam_);

  return true;
}


//-----------------------------------------------------------------------------------------------------------
bool DC1394Camera::setExposureValue(unsigned int value) const
{
  return dc1394_set_exposure(raw1394Handle_, cam_.node, value) == DC1394_SUCCESS;
}


//-----------------------------------------------------------------------------------------------------------
void DC1394Camera::storeFrame(CImg<unsigned char> &frame)
{
  if (mode_ == VIDEOMODE_320x240_YUV422
      || mode_ == VIDEOMODE_640x480_YUV422) {
    // YUV422 mode
    frame.assign(cam_.frame_width, cam_.frame_height, 1, 3);
    unsigned char *pY, *pU, *pV;
    pY = frame.ptr(0, 0, 0, 0);
    pU = frame.ptr(0, 0, 0, 1);
    pV = frame.ptr(0, 0, 0, 2);

    unsigned char *buf = reinterpret_cast<unsigned char*>(cam_.capture_buffer);
    for (int i = 0; i < (cam_.frame_width * cam_.frame_height) / 2; ++i) {
      *(pU++) = *(buf++);
      *(pU++) = *(buf-1);
      *(pY++) = *(buf++);
      *(pV++) = *(buf++);
      *(pV++) = *(buf-1);
      *(pY++) = *(buf++);
    }
  }
  else {
    // RGB mode
    frame.assign(cam_.frame_width, cam_.frame_height, 1, 3);
    unsigned char *pR, *pG, *pB;
    pR = frame.ptr(0, 0, 0, 0);
    pG = frame.ptr(0, 0, 0, 1);
    pB = frame.ptr(0, 0, 0, 2);

    unsigned char *buf = reinterpret_cast<unsigned char*>(cam_.capture_buffer);
    for (int i = 0; i < cam_.frame_width * cam_.frame_height; ++i) {
      *(pR++) = *(buf++);
      *(pG++) = *(buf++);
      *(pB++) = *(buf++);
    }
  }
}


//-----------------------------------------------------------------------------------------------------------
void DC1394Camera::setUp()
{
  raw1394Handle_ = dc1394_create_handle(portNum_);
  if (!raw1394Handle_) {
    ostringstream e;
    e << "Unable to aquire a raw1394 handle for port " << portNum_;
    throw runtime_error(e.str());
  }

  int numNodes = 0;
  int numCams = 0;
  numNodes = raw1394_get_nodecount(raw1394Handle_);
  nodeid_t* camNodes = dc1394_get_camera_nodes(raw1394Handle_, &numCams, 0);

  if (numCams < 1) {
    raw1394_destroy_handle(raw1394Handle_);
    throw runtime_error("No cameras found");
  }

  if (camNum_ > numCams) {
    dc1394_free_camera_nodes(camNodes);
    raw1394_destroy_handle(raw1394Handle_);
    ostringstream e;
    e << "Found cameras [0.." << numCams-1 << "] but you requested camera " << camNum_;
    throw runtime_error(e.str());
  }

  if (camNodes[0] == numNodes-1) {
    dc1394_free_camera_nodes(camNodes);
    raw1394_destroy_handle(raw1394Handle_);
    throw runtime_error("Sorry, your camera is the highest numbered node");
  }

  cam_.node = camNodes[camNum_];
  dc1394_free_camera_nodes(camNodes);

  int mode;
  int framerate;

  switch (mode_) {
    case VIDEOMODE_320x240_YUV422:
      mode = MODE_320x240_YUV422;
      framerate = FRAMERATE_15;
      break;

    case VIDEOMODE_640x480_YUV422:
      mode = MODE_640x480_YUV422;
      framerate = FRAMERATE_15;
      break;

    default:
      mode = MODE_640x480_RGB;
      framerate = FRAMERATE_15;
      break;
  }

  if (dc1394_dma_setup_capture(raw1394Handle_, cam_.node,
        camNum_, /* channel */
        FORMAT_VGA_NONCOMPRESSED,
        mode,
        SPEED_400,
        framerate,
        10, /* number of frames to store in DMA buffer */
        1, /* drop frames */
        NULL, /* path to video device */
        &cam_) != DC1394_SUCCESS) {
    dc1394_dma_release_camera(raw1394Handle_,&cam_);
    raw1394_destroy_handle(raw1394Handle_);
    throw runtime_error("Unable to setup camera");
  }
}


//-----------------------------------------------------------------------------------------------------------
void DC1394Camera::tearDown()
{
  dc1394_stop_iso_transmission(raw1394Handle_, cam_.node);
  dc1394_camera_off(raw1394Handle_, cam_.node);
  dc1394_dma_unlisten(raw1394Handle_,&cam_);
  dc1394_dma_release_camera(raw1394Handle_,&cam_);
  raw1394_destroy_handle(raw1394Handle_);
}



} // namespace rlab
