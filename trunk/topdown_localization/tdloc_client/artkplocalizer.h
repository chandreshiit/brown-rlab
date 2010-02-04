/**
 * Brown University, Computer Science Department
 *
 * Author: Jonas Schwertfeger <js at cs.brown.edu>
 * Date:   9/20/2007
 *
 */

#ifndef ARTKP_LOCALIZER_H
#define ARTKP_LOCALIZER_H ARTKP_LOCALIZER_H

#include "./global.h"
#include "./topdownlocalizer.h"
#include "./localizableobject.h"
#include <iostream>
#include <string>
#include <vector>
#include <sys/time.h>
#include <ARToolKitPlus/TrackerSingleMarkerImpl.h>
#include <ARToolKitPlus/Logger.h>

namespace rlab {

class ARTKPLocalizer : public TopDownLocalizer
{
private:
  class TrackerLogger : public ARToolKitPlus::Logger
  {
    void artLog(const char* nStr) {
      std::cerr << nStr;
    }
  };

public:
  ARTKPLocalizer(
      int imgWidth,
      int imgHeight,
      const std::string& cameraParamFile);
  virtual ~ARTKPLocalizer();

  virtual bool localizeObjects(const Image& img, std::vector<LocalizableObject>& objects);

  void setPatternWidth(float width);
  float getPatternWidth() const;
  void setMinConfidence(float confidence);
  float getMinConfidence() const;
  void setUsingBCH(bool useBCH);
  bool isUsingBCH() const;

private:
  ARToolKitPlus::TrackerSingleMarker* tracker_;
  TrackerLogger logger_;
  std::string cameraParamFile_;
  float patternCenter_[2];
  float patternWidth_;
  float minConfidence_;
  bool usingBCH_;
  float modelViewMatrix_[16];
  struct timeval time_;

  static const float THIN_PATTERN_BORDER;
  static const float THINK_PATTERN_BORDER;
};

} // namespace rlab

#endif // ARTKP_LOCALIZER_H

