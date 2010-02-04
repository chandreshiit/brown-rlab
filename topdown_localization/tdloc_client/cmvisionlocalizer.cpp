/**
 * Brown University, Computer Science Department
 *
 * Author: Jonas Schwertfeger <js at cs.brown.edu>
 * Date:   10/8/2007
 *
 */
#include "./cmvisionlocalizer.h"
#include "./objectpose.h"
#include <string>
#include <string.h>
#include <sstream>
#include <stdexcept>
#include <cmath>

using namespace std;

namespace rlab {

//-----------------------------------------------------------------------------------------------------------
CMVisionLocalizer::CMVisionLocalizer(int imgWidth, int imgHeight, const string& colorFile)
  : TopDownLocalizer(imgWidth, imgHeight),
    cmVision_(NULL),
    colorFile_(colorFile)
{
  cmVision_ = new CMVision();

  if (!cmVision_->initialize(getImageWidth(), getImageHeight())) {
    ostringstream e;
    e << "Failed to initialize CMVision.";
    throw runtime_error(e.str());
  }

  if (!cmVision_->loadOptions(colorFile_.c_str())) {
    ostringstream e;
    e << "Failed to load colors file " << colorFile_ << ".";
    throw runtime_error(e.str());
  }

  // Find out how many colors CMVision read in
  numColors_ = 0;
  while (numColors_ < 32 && cmVision_->getColorInfo(numColors_)->name) {
    ++numColors_;
  }

  // Find ball color
  ballColor_ = 0;
  while (ballColor_ < numColors_ && strcasecmp(cmVision_->getColorInfo(ballColor_)->name, "ball") != 0) {
    ++ballColor_;
  }

  //cout << "Num colors read: " << numColors_ << endl;
  //cout << "Ball color index: " << ballColor_ << endl;
}

//-----------------------------------------------------------------------------------------------------------
CMVisionLocalizer::~CMVisionLocalizer()
{
  if (cmVision_) {
    delete cmVision_;
  }
}

//-----------------------------------------------------------------------------------------------------------
bool CMVisionLocalizer::localizeObjects(const Image& img, vector<LocalizableObject>& objects)
{
  vector<Blob> dummy;
  return localizeObjects(img, objects, dummy);
}


//-----------------------------------------------------------------------------------------------------------
bool CMVisionLocalizer::localizeObjects(
    const Image& img,
    vector<LocalizableObject>& objects,
    vector<Blob>& blobs)
{
  if (!cmVision_->processFrame(img)) {
    cerr << "CMVision failed to process frame." << endl;
    return false;
  }

  Blob blob;

  // Check whether the ball was spotted
  int numBallRegions = cmVision_->numRegions(ballColor_);
  if (numBallRegions) {
    // Find the largest ball region
    CMVision::region* regions = cmVision_->getRegions(ballColor_);
    int largest = 0;
    int largestArea = 0;
    for (int i = 0; i < numBallRegions; ++i) {
      /*blob.x1 = regions[i].x1;
      blob.y1 = regions[i].y1;
      blob.x2 = regions[i].x2;
      blob.y2 = regions[i].y2;
      blob.color = regions[i].color;
      blobs.push_back(blob);*/

      if (regions[i].area > largestArea) {
        largest = i;
        largestArea = regions[i].area;
      }
    }

    // Ball blob should have a reasonable area (this discards sensor noise)
    if (largestArea >= 100) {
      blob.x1 = regions[largest].x1;
      blob.y1 = regions[largest].y1;
      blob.x2 = regions[largest].x2;
      blob.y2 = regions[largest].y2;
      blob.color = regions[largest].color;
      blobs.push_back(blob);

      LocalizableObject object;
      object.setType(LocalizableObject::OBJECT_TYPE_BALL);
      object.setId(1);
      object.setPose(computePoseFromRegion(regions[largest]));
      object.setPoseAge(0);
      objects.push_back(object);

      //cout << "Ball:" << endl;
      //cout << "X: " << object.getPose().getX() << endl;
      //cout << "Y: " << object.getPose().getY() << endl;
      //cout << endl;
    }
  }

  return true;
}


//-----------------------------------------------------------------------------------------------------------
void CMVisionLocalizer::setHorizontalFoV(float fov)
{
  if (fov <= 0) {
    return;
  }

  horizontalFoV_ = fov;
}


//-----------------------------------------------------------------------------------------------------------
float CMVisionLocalizer::getHorizontalFoV() const
{
  return horizontalFoV_;
}


//-----------------------------------------------------------------------------------------------------------
void CMVisionLocalizer::setVerticalFoV(float fov)
{
  if (fov <= 0) {
    return;
  }

  verticalFoV_ = fov;
}


//-----------------------------------------------------------------------------------------------------------
float CMVisionLocalizer::getVerticalFoV() const
{
  return verticalFoV_;
}


//-----------------------------------------------------------------------------------------------------------
ObjectPose CMVisionLocalizer::computePoseFromRegion(const CMVision::region& region) const
{
  ObjectPose pose;
  pose.setX((region.cen_x / getImageWidth()) * getHorizontalFoV() - (getHorizontalFoV() / 2) + getXOffset());
  pose.setY((region.cen_y / getImageHeight()) * getVerticalFoV() - (getVerticalFoV() / 2) + getYOffset());
  return pose;
}

} // namespace rlab

