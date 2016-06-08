#include "ColorModel.h"
#include "Tools/ColorModelConversions.h"
#include "Tools/File.h"
#include <fstream>

static class ColorSpaceMapper
{
public:
  struct RB
  {
    unsigned char r;
    unsigned char b;
    unsigned short rb;
  };
  
  cv::Vec3b hsi[32][256][256];
  RB rb[32][256][256];
  
  ColorSpaceMapper()
  {
    cv::Vec3b* p = &hsi[0][0][0];
    RB* q = &rb[0][0][0];
    unsigned char g;
    for(int y = 0; y < 256; y += 8)
      for(int cb = 0; cb < 256; ++cb)
        for(int cr = 0; cr < 256; ++cr, ++p, ++q)
        {
          ColorModelConversions::fromYCbCrToHSI((unsigned char) y,
                                                (unsigned char) cb,
                                                (unsigned char) cr,
                                                (*p)[0], (*p)[1], (*p)[2]);
          ColorModelConversions::fromYCbCrToRGB((unsigned char) y,
                                                (unsigned char) cb,
                                                (unsigned char) cr,
                                                q->r, g, q->b);
          q->rb = (unsigned short) q->r + q->b;
        }
  }
} colorSpaceMapper;

void ColorModel::fromColorCalibration(const ColorCalibration& colorCalibration, ColorCalibration& prevCalibration)
{
  bool greenChanged = colorCalibration.ranges[green] != prevCalibration.ranges[green];
  for(unsigned char i = 2; i < numOfColors; ++i)
    if(colorCalibration.ranges[i] != prevCalibration.ranges[i])
    {
      setCube(colorCalibration.ranges[i],Colors((Color)i));
      prevCalibration.ranges[i] = colorCalibration.ranges[i];
    }
  
  if(colorCalibration.whiteThreshold != prevCalibration.whiteThreshold || greenChanged)
  {
    setCube(colorCalibration.whiteThreshold,Color(white));
    prevCalibration.whiteThreshold = colorCalibration.whiteThreshold;
  }
}

void ColorModel::setCube(const ColorCalibration::HSIRanges& ranges, Colors color)
{
  Colors* dest = &cubo[0][0][0];
  unsigned char setColor = color.colors;
  for (int y = 0; y < 32; y++) {
    for (int cb = 0; cb < 256; cb++) {
      for (int cr = 0; cr < 256; cr++, dest++) {
        cv::Vec3b hsi = colorSpaceMapper.hsi[y][cb][cr];
        if (ranges.hue.isInside(hsi[0]) && ranges.saturation.isInside(hsi[1]) && ranges.intensity.isInside(hsi[2])) {
          dest->colors |= setColor;
        }
        else
          dest->colors &= ~setColor;
      }
    }
  }
}

void ColorModel::setCube(const ColorCalibration::WhiteThresholds& thresholds, Colors color)
{
  Colors* dest = &cubo[0][0][0];
  unsigned char setColor = color.colors;
  for (int y = 0; y < 32; y++) {
    for (int cb = 0; cb < 256; cb++) {
      for (int cr = 0; cr < 256; cr++, dest++) {
        ColorSpaceMapper::RB rb = colorSpaceMapper.rb[y][cb][cr];
        if (rb.b >= thresholds.minB && rb.r >= thresholds.minR && rb.b + rb.r >= thresholds.minRB && !(dest->colors & 1 << (green -1))) {
          dest->colors |= setColor;
        }
        else
          dest->colors &= ~setColor;
      }
    }
  }
}

ColorModel::Colors ColorModel::getColor(cv::Vec3b point) const
{
  return cubo[point[0] >> 3][point[2]][point[1]];
}
