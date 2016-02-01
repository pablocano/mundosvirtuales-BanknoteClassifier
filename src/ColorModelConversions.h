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
  static void fromBGRToHSI(unsigned char B,
                             unsigned char G,
                             unsigned char R,
                             unsigned char& H,
                             unsigned char& S,
                             unsigned char& I)
{
		if(B == 50 && G == 200 && R == 130)
			int x = 1283;
		
		float b = 0, g = 0, r = 0,h,s,i;
	
		float sum = B + G + R;
		if(sum){
			b = B/sum;
			g = G/sum;
			r = R/sum;
		}
		i = sum / (3.f/255.f);
		
		float min_val = 0;
		min_val = std::min(r, std::min(b,g));
		
		s = 0;
		if (b+g+r) {
			s = 1.f - 3.f*(min_val/(b + g + r));
		}
		
		
		if(s < 0.00001f)
		{
			s = 0;
		}else if(s > 0.99999f){
			s = 1;
		}
		h = 0;
		if(s != 0)
		{
			h = std::sqrt(((r - g)*(r - g)) + ((r - b)*(g - b)));
			if (h) {
				h = 0.5 * ((r - g) + (r - b)) / h;
				h = std::acos(h);
			}
			
			if(b > g)
			{
				h = 2.f*M_PI - h;
			}
		}
		H = (unsigned char)((h * 180.f/M_PI)*(255.f/360.f));
		S = (unsigned char)(s*255.f);
		I = (unsigned char)(i*255.f);
  }
};
