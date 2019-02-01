/**
 * @file ModuleManager.cpp
 * Implementation of a class representing the module manager.
 * @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
 */

#include "ModuleManager.h"
#include <algorithm>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "Tools/File.h"

ModuleManager::Configuration::RepresentationProvider::RepresentationProvider(const std::string& representation,
                                                                             const std::string& provider)
: representation(representation),
  provider(provider) {}

ModuleManager::ModuleManager(const std::set<std::string>& categories)
{
  for(ModuleBase* i = ModuleBase::first; i; i = i->next)
    if(categories.find(i->category) != categories.end())
      modules.push_back(ModuleState(i));
    else
      otherModules.push_back(ModuleState(i));
}

ModuleManager::~ModuleManager()
{
  destroy();
}

void ModuleManager::destroy()
{
  if(!providers.empty())
  {
    providers.clear();
    
    // Remove all markings
    for(auto& m : modules)
    {
      m.requiredBackup = m.required;
      m.required = false;
    }
    
    // Delete all modules that are not required anymore
    for(auto& m : modules)
      if(!m.required && m.instance)
      {
        delete m.instance;
        m.instance = 0;
      }
  }
}

const ModuleBase::Info* ModuleManager::find(const ModuleBase* module, const std::string& representation, bool all)
{
  for(const ModuleBase::Info* i = module->info; i->representation; ++i)
    if((i->update || all) && representation == i->representation)
      return i;
  return 0;
}

bool ModuleManager::sortProviders(std::list<std::string> providedByDefault)
{
  // The representations already provided. These are all that are received from the other process
  // and the ones that are provided by default.
  std::list<std::string> provided = providedByDefault;

  int remaining = (int) providers.size(), // The number of entries not correct sequence so far.
      pushBackCount = remaining; // The number of push_backs still allowed. If zero, no valid sequence is possible.
  std::list<Provider>::iterator i = providers.begin();
  while(i != providers.end())
  {
    const ModuleBase::Info* j;
    for(j = i->moduleState->module->info; j->representation; ++j)
      if(!j->update && j->representation != i->representation && std::find(provided.begin(), provided.end(), j->representation) == provided.end())
        break;
    if(i->moduleState->module->info->representation && j->representation) // at least one requirement missing
    {
      if(pushBackCount) // still one left to try
      {
        providers.push_back(*i);
        i = providers.erase(i);
        --pushBackCount;
      }
      else // we checked all, none was satisfied
      {
        std::string text;
        for(const auto& p : provided)
        {
          text += text == "" ? "" : ", ";
          text += p;
        }
        std::string text2 = "";
        while(i != providers.end())
        {
          text2 += text2 == "" ? "" : ", ";
          text2 += i->representation;
          ++i;
        }
        if(text == "")
          std::cout << "requirements missing for providers for " << text2 << ".";
        else
          std::cout << "only found consistent providers for " << text <<
                       ".\nRequirements missing for providers for " << text2 << ".";
        return false;
      }
    }
    else // we found one with all requirements fulfilled
    {
      provided.push_back(i->representation); // add representation provided
      ++i; // continue with next provider
      --remaining; // we have one less to go,
      pushBackCount = remaining; // and the search starts again
    }
  }
  return true;
}

void ModuleManager::rollBack(const std::list<Provider>& providers)
{
  this->providers = providers;
  for(auto& m : modules)
    m.required = m.requiredBackup;
}

void ModuleManager::load()
{
  std::list<Provider> providersBackup(providers);
  
  std::string representation,
  module;
  
  providers.clear();
  
  // Remove all markings
  for(auto& m : modules)
  {
    m.requiredBackup = m.required;
    m.required = false;
  }

  config.load("modules.cfg");


  std::list<std::string> providedByDefault;

  for(const auto& rp : config.representationProviders)
  {
      if(rp.provider == "default")
          providedByDefault.push_back(rp.representation);
      else
          for(auto& m : modules)
              if(rp.provider == m.module->name)
              {
                  for(const ModuleBase::Info* i = m.module->info; i->representation; ++i)
                      if(i->update && rp.representation == i->representation)
                      {
                          providers.push_back(Provider(i->representation, &m, i->update));
                          break;
                      }
                  m.required = true;
                  break;
              }
  }
  
  if(!sortProviders(providedByDefault))
  {
    return;
  }
  
  // Delete all modules that are not required anymore
  for(auto& m : modules)
    if(!m.required && m.instance)
    {
      delete m.instance;
      m.instance = 0;
    }
}

void ModuleManager::execute()
{
  // Execute all providers in the given sequence
  for(auto& p : providers)
    if(p.moduleState->required)
    {
      if(!p.moduleState->instance)
        p.moduleState->instance = p.moduleState->module->createNew(); // returns 0 if provided by "default"
      if(p.moduleState->instance)
        p.update(*p.moduleState->instance);

    }
}

void ModuleManager::Configuration::save(std::string filename)
{
    cv::FileStorage fs(std::string(File::getGTDir()) + "/Config/" + filename, cv::FileStorage::WRITE);

    fs << "representationProviders" << "[";

    for(const auto& rp: representationProviders)
    {
        fs << "{:" << "representation" << rp.representation << "provider" << rp.provider << "}";
    }

    fs << "]";
    fs.release();
}

bool ModuleManager::Configuration::load(std::string filename)
{
    cv::FileStorage fs(std::string(File::getGTDir()) + "/Config/" + filename, cv::FileStorage::READ);


    cv::FileNode rpProv = fs["representationProviders"];
    cv::FileNodeIterator it = rpProv.begin(), it_end = rpProv.end();

    representationProviders.clear();

    for(; it != it_end; ++it)
    {
        representationProviders.push_back(RepresentationProvider((std::string)(*it)["representation"],(std::string)(*it)["provider"]));
    }

    fs.release();

    return true;

}
