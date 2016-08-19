
#pragma once

#include "Tools/ColorClasses.h"
#include "Tools/Math/Range.h"
#include "Tools/Streamable.h"
#include <opencv2/core/core.hpp>

class ColorCalibration : public Streamable
{
public:
	
	class WhiteThresholds
	{
	public:
		WhiteThresholds() : minR(0),minB(0),minRB(0) {}
		
		WhiteThresholds(int r, int b, int rb) : minR(r), minB(b), minRB(rb) {}
		
		bool operator==(const WhiteThresholds& other) const
		{
			return minR == other.minR &&
			minB == other.minB &&
			minRB == other.minRB;
		}
		
		bool operator!=(const WhiteThresholds& other) const
		{
			return !(*this == other);
		}
    
    void write(cv::FileStorage& fs) const
    {
      fs << "{" << "minR" << minR << "minB" << minB << "minRB" << minRB << "}";
    }
    
    void read(const cv::FileNode& node)
    {
      minR = (int)node["minR"];
      minB = (int)node["minB"];
      minRB = (int)node["minRB"];
    }
		
		int minR;
		int minB;
		int minRB;
	};
	
	class HSIRanges
	{
	public:
		HSIRanges() : hue(0), saturation(0), intensity(0) {}
		
		HSIRanges(Range<int> h, Range<int> s, Range<int> i): hue(h.min,h.max), saturation(s.min,s.max), intensity(i.min,i.max) {}
		
		bool operator==(const HSIRanges& other) const
		{
			return hue == other.hue && saturation == other.saturation && intensity == other.intensity;
		}
							 
		bool operator!=(const HSIRanges& other) const
		{
			return !(*this == other);
		}
    
    void write(cv::FileStorage& fs) const
    {
      fs << "{" << "hue" << hue << "saturation" << saturation << "intensity" << intensity << "}";
    }
    
    void read(const cv::FileNode& node)
    {
      node["hue"] >> hue;
      node["saturation"] >> saturation;
      node["intensity"] >> intensity;
    }
		
		Range<int> hue;
		Range<int> saturation;
		Range<int> intensity;
	};
  
  void write(cv::FileStorage& fs) const
  {
    fs << "{" << "whiteThreshold" << whiteThreshold << "ranges" << "[";
    
    for (int i = 0; i < numOfColors; i++) {
      fs << ranges[i];
    }
    
    fs << "]" << "}";
  }
  
  void read(const cv::FileNode& node)
  {
    node["whiteThreshold"] >> whiteThreshold;
    const cv::FileNode readRanges = node["ranges"];
    cv::FileNodeIterator it = readRanges.begin(), it_end = readRanges.end(); // Go through the node
    for (int i = 0; it != it_end; ++it, i++)
    {
      (*it) >> ranges[i];
    }
    
  }
	
	WhiteThresholds whiteThreshold;
	HSIRanges ranges[numOfColors];
};

static void write(cv::FileStorage& fs, const std::string&, const ColorCalibration::WhiteThresholds& x)
{
  x.write(fs);
}
static void read(const cv::FileNode& node, ColorCalibration::WhiteThresholds& x, const ColorCalibration::WhiteThresholds& default_value = ColorCalibration::WhiteThresholds())
{
  if(node.empty())
    x = default_value;
  else
    x.read(node);
}

static void write(cv::FileStorage& fs, const std::string&, const ColorCalibration::HSIRanges& x)
{
  x.write(fs);
}
static void read(const cv::FileNode& node, ColorCalibration::HSIRanges& x, const ColorCalibration::HSIRanges& default_value = ColorCalibration::HSIRanges())
{
  if(node.empty())
    x = default_value;
  else
    x.read(node);
}
static void write(cv::FileStorage& fs, const std::string&, const ColorCalibration& x)
{
  x.write(fs);
}
static void read(const cv::FileNode& node, ColorCalibration& x, const ColorCalibration& default_value = ColorCalibration())
{
  if(node.empty())
    x = default_value;
  else
    x.read(node);
}