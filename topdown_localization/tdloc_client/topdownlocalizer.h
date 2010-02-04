/**
 * Brown University, Computer Science Department
 *
 * Author: Jonas Schwertfeger <js at cs.brown.edu>
 * Date:   9/20/2007
 *
 */

#ifndef TOP_DOWN_LOCALIZER_H
#define TOP_DOWN_LOCALIZER_H TOP_DOWN_LOCALIZER_H

#include "./global.h"
#include "./localizableobject.h"
#include <vector>

namespace rlab {

class TopDownLocalizer
{
public:
  TopDownLocalizer(int imgWidth, int imgHeight);
  virtual ~TopDownLocalizer();

  virtual bool localizeObjects(const Image& img, std::vector<LocalizableObject>& objects) = 0;

  void setImageWidth(int width);
  int getImageWidth() const;

  void setImageHeight(int height);
  int getImageHeight() const;

  void setXOffset(float offset);
  float getXOffset() const;

  void setYOffset(float offset);
  float getYOffset() const;

protected:
  int imgWidth_;
  int imgHeight_;

  float xOffset_;
  float yOffset_;
};

} // namespace rlab

#endif // TOP_DOWN_LOCALIZER_H

