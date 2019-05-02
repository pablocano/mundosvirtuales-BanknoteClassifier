#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/CameraInfo.h"
#include "Representations/CameraSettings.h"
#include "Representations/ColorModel/ColorCalibration.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Representations/FrameInfo.h"
#include "Tools/MessageQueue/InMessage.h"
#include "memory.h"

MODULE(BanknoteCorrectorConfiguration,
{,
  PROVIDES(CorrectorFrameInfo),
  PROVIDES(CorrectorCameraSettings),
});

class BanknoteCorrectorConfiguration : public BanknoteCorrectorConfigurationBase
{
private:

    static thread_local BanknoteCorrectorConfiguration *theInstance;

    std::unique_ptr<CorrectorCameraSettings> theCorrectorCameraSettings;

    void update(CorrectorFrameInfo& frameInfo) override;

    void update(CorrectorCameraSettings& cameraSettings) override {update(cameraSettings, theCorrectorCameraSettings);}

    template<typename T> void update(T& representation, std::unique_ptr<T>& theRepresentation)
    {
      if(theRepresentation)
      {
        representation = *theRepresentation;
        theRepresentation = nullptr;
      }
    }

    template<typename T> void read(std::unique_ptr<T>& theRepresentation, const char* fileName = nullptr)
    {
      ASSERT(!theRepresentation);
      theRepresentation = std::make_unique<T>();
      loadModuleParameters(*theRepresentation, TypeRegistry::demangle(typeid(T).name()).c_str(), fileName);
    }
  
    unsigned last;
  
public:
  
  BanknoteCorrectorConfiguration();
  
  static bool handleMessage(InMessage& message);
};
