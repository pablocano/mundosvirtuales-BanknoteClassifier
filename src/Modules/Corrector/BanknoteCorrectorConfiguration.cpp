#include "BanknoteCorrectorConfiguration.h"
#include "Platform/SystemCall.h"
#include "Tools/Comm/BanknoteClassifierMessageHandler.h"
#include "Tools/Debugging/Debugging.h"
#include "Platform/File.h"

MAKE_MODULE(BanknoteCorrectorConfiguration, BanknoteCorrector)

BanknoteCorrectorConfiguration* BanknoteCorrectorConfiguration::theInstance = 0;

BanknoteCorrectorConfiguration::BanknoteCorrectorConfiguration()
{
  theInstance = this;
  
  last = SystemCall::getCurrentSystemTime();
}

void BanknoteCorrectorConfiguration::update(CorrectorFrameInfo& frameInfo)
{
  frameInfo.time += SystemCall::getTimeSince(last);
  last = SystemCall::getCurrentSystemTime();
}

bool BanknoteCorrectorConfiguration::handleMessage(InMessage& message)
{
}
