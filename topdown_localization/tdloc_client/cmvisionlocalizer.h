/**
 * Brown University, Computer Science Department
 *
 * Author: Jonas Schwertfeger <js at cs.brown.edu>
 * Date:   10/8/2007
 *
 */

#ifndef CMVISION_LOCALIZER_H
#define CMVISION_LOCALIZER_H CMVISION_LOCALIZER_H

#include "./global.h"
#include "./topdownlocalizer.h"
#include "./localizableobject.h"
#include "./objectpose.h"
#include "./cmvision.h"
#include <iostream>
#include <string>
#include <vector>
#include <sys/time.h>

namespace rlab {

class CMVisionLocalizer : public TopDownLocalizer {
public:
  CMVisionLocalizer(
      int imgWidth,
      int imgHeight,
      const std::string& colorFile);
  virtual ~CMVisionLocalizer();

  virtual bool localizeObjects(const Image& img, std::vector<LocalizableObject>& objects);
  bool localizeObjects(const Image& img, std::vector<LocalizableObject>& objects, std::vector<Blob>& blobs);

  void setHorizontalFoV(float fov);
  float getHorizontalFoV() const;

  void setVerticalFoV(float fov);
  float getVerticalFoV() const;

private:
  ObjectPose computePoseFromRegion(const CMVision::region& region) const;

private:
  CMVision* cmVision_;
  std::string colorFile_;
  int numColors_;
  int ballColor_;

  float horizontalFoV_;
  float verticalFoV_;
};

} // namespace rlab

#endif // CMVISION_LOCALIZER_H

