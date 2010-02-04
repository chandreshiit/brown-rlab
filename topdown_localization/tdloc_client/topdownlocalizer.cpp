/**
 * Brown University, Computer Science Department
 *
 * Author: Jonas Schwertfeger <js at cs.brown.edu>
 * Date:   9/20/2007
 *
 */

#include "./topdownlocalizer.h"

namespace rlab {

//-----------------------------------------------------------------------------------------------------------
TopDownLocalizer::TopDownLocalizer(int imgWidth, int imgHeight)
  : imgWidth_(imgWidth),
    imgHeight_(imgHeight),
    xOffset_(0.0),
    yOffset_(0.0)
{
}


//-----------------------------------------------------------------------------------------------------------
TopDownLocalizer::~TopDownLocalizer()
{
}


//-----------------------------------------------------------------------------------------------------------
void TopDownLocalizer::setImageWidth(int width)
{
  imgWidth_ = width;
}


//-----------------------------------------------------------------------------------------------------------
int TopDownLocalizer::getImageWidth() const
{
  return imgWidth_;
}


//-----------------------------------------------------------------------------------------------------------
void TopDownLocalizer::setImageHeight(int height)
{
  imgHeight_ = height;
}


//-----------------------------------------------------------------------------------------------------------
int TopDownLocalizer::getImageHeight() const
{
  return imgHeight_;
}


//-----------------------------------------------------------------------------------------------------------
void TopDownLocalizer::setXOffset(float offset)
{
  xOffset_ = offset;
}


//-----------------------------------------------------------------------------------------------------------
float TopDownLocalizer::getXOffset() const
{
  return xOffset_;
}


//-----------------------------------------------------------------------------------------------------------
void TopDownLocalizer::setYOffset(float offset)
{
  yOffset_ = offset;
}


//-----------------------------------------------------------------------------------------------------------
float TopDownLocalizer::getYOffset() const
{
  return yOffset_;
}

} // namespace rlab

