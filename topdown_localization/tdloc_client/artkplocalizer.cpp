/**
 * Brown University, Computer Science Department
 *
 * Author: Jonas Schwertfeger <js at cs.brown.edu>
 * Date:   9/20/2007
 *
 */

#include "./artkplocalizer.h"
#include "./objectpose.h"
#include <string>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <ARToolKitPlus/CameraFactory.h>
#include <ARToolKitPlus/Camera.h>
#include <ARToolKitPlus/ar.h>

using namespace std;

namespace rlab {

const float ARTKPLocalizer::THIN_PATTERN_BORDER = 0.125;
const float ARTKPLocalizer::THINK_PATTERN_BORDER = 0.25;

//-----------------------------------------------------------------------------------------------------------
ARTKPLocalizer::ARTKPLocalizer(
    int imgWidth,
    int imgHeight,
    const string& cameraParamFile)
  : TopDownLocalizer(imgWidth, imgHeight),
    tracker_(NULL),
    logger_(),
    cameraParamFile_(cameraParamFile),
    patternWidth_(80.0),
    minConfidence_(0.5)
{
  patternCenter_[0] = patternCenter_[1] = 0.0;

  // Create a tracker that
  //  - detects 6x6 sized marker images
  //  - samples at a maximum of 6x6
  //  - can load a maximum of 1 patterns
  //  - can detect a maximum of 8 patterns in one image
  //  - with an arbitrary default image size
  tracker_ = new ARToolKitPlus::TrackerSingleMarkerImpl<6, 6, 6, 1, 8>(getImageWidth(), getImageHeight());
	tracker_->setPixelFormat(ARToolKitPlus::PIXEL_FORMAT_LUM);
  tracker_->setLogger(&logger_);

	// Load NULL camera
  if (!tracker_->init(NULL, 0.0, 0.0)) {
    ostringstream e;
    e << "Failed to initialize tracker with NULL camera.";
    throw runtime_error(e.str());
	}

  // Load camera specific parameter file
  ARToolKitPlus::CameraFactory cameraFactory;
  ARToolKitPlus::Camera* camera = cameraFactory.createCamera(cameraParamFile_.c_str());
  if (!camera) {
    ostringstream e;
    e << "Failed to load camera parameter file " << cameraParamFile_ << ".";
    throw runtime_error(e.str());
  }
  tracker_->setCamera(camera);

  tracker_->setUndistortionMode(ARToolKitPlus::UNDIST_NONE);
  tracker_->activateAutoThreshold(true);
  tracker_->setPoseEstimator(ARToolKitPlus::POSE_ESTIMATOR_RPP);
  setUsingBCH(true);
}


//-----------------------------------------------------------------------------------------------------------
ARTKPLocalizer::~ARTKPLocalizer()
{
  if (tracker_) {
    tracker_->cleanup();
    delete tracker_;
  }
}


//-----------------------------------------------------------------------------------------------------------
bool ARTKPLocalizer::localizeObjects(const Image& img, vector<LocalizableObject>& objects)
{
  int numMarkers = 0;
  ARToolKitPlus::ARMarkerInfo* markers = NULL;
  if (tracker_->arDetectMarker(const_cast<unsigned char*>(img.ptr()), 0, &markers, &numMarkers) < 0) {
    cerr << "Failed to detect markers." << endl;
    return false;
  }

  for(int m = 0; m < numMarkers; ++m) {
    if(markers[m].id != -1 && markers[m].cf >= minConfidence_) {
      tracker_->calcOpenGLMatrixFromMarker(&markers[m], patternCenter_,
          patternWidth_, modelViewMatrix_);

      LocalizableObject object;
      object.setId(markers[m].id);
      if (object.getId() < 10) {
        object.setType(LocalizableObject::OBJECT_TYPE_SMURV);
      }
      else {
        object.setType(LocalizableObject::OBJECT_TYPE_OBSTACLE);
      }

      float x = modelViewMatrix_[12] / 1000.0;
      float y = modelViewMatrix_[13] / 1000.0;
      float yaw = atan2(modelViewMatrix_[1], modelViewMatrix_[0]);

      if (x == 0 && y == 0 && yaw == 0) {
        // ARTKPlus bug that occurs sometimes
        continue;
      }
      object.setPose(ObjectPose(x + xOffset_, y + yOffset_, yaw));
      object.setPoseAge(0);
      objects.push_back(object);
    }
  }

  return true;
}


//-----------------------------------------------------------------------------------------------------------
void ARTKPLocalizer::setPatternWidth(float width)
{
  patternWidth_ = width;
}


//-----------------------------------------------------------------------------------------------------------
float ARTKPLocalizer::getPatternWidth() const
{
  return patternWidth_;
}


//-----------------------------------------------------------------------------------------------------------
void ARTKPLocalizer::setMinConfidence(float confidence)
{
  minConfidence_ = confidence;
}


//-----------------------------------------------------------------------------------------------------------
float ARTKPLocalizer::getMinConfidence() const
{
  return minConfidence_;
}


//-----------------------------------------------------------------------------------------------------------
void ARTKPLocalizer::setUsingBCH(bool useBCH)
{
  usingBCH_ = useBCH;

  if (usingBCH_) {
    tracker_->setMarkerMode(ARToolKitPlus::MARKER_ID_BCH);
    tracker_->setBorderWidth(THIN_PATTERN_BORDER);
  }
  else {
    tracker_->setMarkerMode(ARToolKitPlus::MARKER_ID_SIMPLE);
    tracker_->setBorderWidth(THINK_PATTERN_BORDER);
  }
}


//-----------------------------------------------------------------------------------------------------------
bool ARTKPLocalizer::isUsingBCH() const
{
  return usingBCH_;
}

} // namespace rlab

