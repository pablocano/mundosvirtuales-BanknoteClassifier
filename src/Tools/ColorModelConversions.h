/**
 * @file ColorModelConversions.h
 *
 * Declaration and implementation of class ColorModelConversions
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <cstdlib>

/**
* A class that defines static conversions between color models for single pixels.
*/
class ColorModelConversions
{
public:
    /** Converts an YCbCr pixel into an RGB pixel.
     *  @param Y The Y channel of the source pixel.
     *  @param Cb The Cb channel of the source pixel.
     *  @param Cr The Cr channel of the source pixel.
     *  @param R The R channel of the target pixel.
     *  @param G The G channel of the target pixel.
     *  @param B The B channel of the target pixel.
     */
    static void fromYCbCrToRGB(unsigned char Y,
                                 unsigned char Cb,
                                 unsigned char Cr,
                                 unsigned char& R,
                                 unsigned char& G,
                                 unsigned char& B)
    {
        int r = Y + ((1436 * (Cr - 128)) >> 10),
            g = Y - ((354 * (Cb - 128) + 732 * (Cr - 128)) >> 10),
            b = Y + ((1814 * (Cb - 128)) >> 10);
        if(r < 0) r = 0;
        else if(r > 255) r = 255;
        if(g < 0) g = 0;
        else if(g > 255) g = 255;
        if(b < 0) b = 0;
        else if(b > 255) b = 255;
        R = (unsigned char) r;
        G = (unsigned char) g;
        B = (unsigned char) b;
      }

  /** Converts an YCbCr pixel into an RGB pixel.
   *  @param Y The Y channel of the source pixel.
   *  @param Cb The Cb channel of the source pixel.
   *  @param Cr The Cr channel of the source pixel.
   *  @param R The R channel of the target pixel.
   *  @param G The G channel of the target pixel.
   *  @param B The B channel of the target pixel.
   */
  static void fromYCbCrToHSI(unsigned char Y,
                             unsigned char Cb,
                             unsigned char Cr,
                             unsigned char& H,
                             unsigned char& S,
                             unsigned char& I)
    {
      int r = Y + ((1436 * (Cr - 128)) >> 10);
          int g = Y - ((354 * (Cb - 128) + 732 * (Cr - 128)) >> 10);
          int b = Y + ((1814 * (Cb - 128)) >> 10);
          int k = 0;

          if(g < b)
          {
            std::swap(g, b);
            k = -256;
          }

          if(r < g)
          {
            std::swap(r, g);
            k = -85 - k;
          }

          int chroma = r - std::min(g, b);
          int h = std::abs(k + ((g - b) << 8) / (chroma ? 6 * chroma : 1));
          int s = (chroma << 8) / (r ? r : 1);

          // normalize values to their boundaries
          H = (unsigned char) std::min(255, h);
          S = (unsigned char) std::max(0, std::min(255, s));
          I = (unsigned char) std::max(0, std::min(255, r));
    }
};
