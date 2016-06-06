/**
 * @file ModuleManager.h
 * Declaration of a class representing the module manager.
 * @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
 */

#pragma once

#include "Module.h"
#include <list>
#include <map>
#include <set>
#include <vector>

/**
 * @class ModuleManager
 * A class representing the module manager.
 */
class ModuleManager
{
private:
  /**
   * The class represents the current state of a module.
   */
  class ModuleState
  {
  public:
    ModuleBase* module; /**< A pointer to the module base that is able to create an instance of the module. */
    Streamable* instance; /**< A pointer to the instance of the module if it was created. Otherwise the pointer is 0. */
    bool required; /**< A flag that is required when determining whether a module is currently required or not. */
    bool requiredBackup; /**< Temporary backup of "required" */

    /**
     * Constructor.
     * @param module A pointer to the module base that is able to create an instance of the module.
     */
    ModuleState(ModuleBase* module) : module(module), instance(0), required(false) {}

    /**
     * Comparison operator. Only uses the name for comparison.
     * @param name The module name this one is compared to.
     * @return Is this the name of the module?
     */
    bool operator==(const std::string& name) const {return module->name == name;}
  };

  /**
   * The class represents a provider of information, i.e. a method of a module that updates a representation.
   */
  class Provider
  {
  public:
    const char* representation; /**< The representation that will be provided. */
    ModuleState* moduleState; /**< The moduleState that will give access to the module that provides the information. */
    void (*update)(Streamable&); /**< The update handler within the module. */

    /**
     * Constructor.
     * @param representation The name of the representation provided.
     * @param moduleState The moduleState that will give access to the module that provides the information.
     * @param update The update handler within the module.
     * @param create The create handler for the representation.
     * @param free The free handler for the representation.
     */
    Provider(const char* representation, ModuleState* moduleState,
             void (*update)(Streamable&))
    : representation(representation),
      moduleState(moduleState),
      update(update) {}
  };

public:
  /**
   * The class is used for reading configurations.
   *
   * This class is public since it is used for the ModuleInfo also.
   */
  class Configuration
  {
  public:
    class RepresentationProvider
    {
    public:
      RepresentationProvider() = default;
      RepresentationProvider(const std::string& representation, const std::string& provider);

      /**
       * Comparison operator. Uses the representation name for comparison.
       * @param other The other RepresentationProvider this one is compared to.
       * @return Is this representations name lower than the other?
       */
      bool operator<(const RepresentationProvider& other) const
      {
        return (representation < other.representation);
      };

      std::string representation;
      std::string provider;
    };

    std::vector<RepresentationProvider> representationProviders;
  };

private:
  Configuration config; /**< The last configuration set. It may not work. */
  std::list<Provider> providers; /**< The list of providers that will be executed. */
  std::list<ModuleState> modules; /**< The current state of all modules. */
  std::list<ModuleState> otherModules; /**< The modules in other processes. */
  /**
   * Find information about a representation provided or required by a certain module.
   * @param module The module that is searched for the representation.
   * @param representation The name of the representation that is searched for.
   * @param all If false, it is only searched for representations that are provided by this module.
   *            Otherwise, it is also searched for presentations that are required.
   * @return Information found about the representation. 0 if it was not found.
   */
  static const ModuleBase::Info* find(const ModuleBase* module, const std::string& representation, bool all = false);
  
  /**
   * The method brings the providers in the correct sequence.
   * @return Is the set of providers consistent?
   */
  bool sortProviders();

  /**
   * The method restores a previous module configuration.
   * It is called after it was determined that the new configuration is invalid.
   * @param providers The previous providers.
   * @param sent The previous list of represenations to send to the other process.
   * @param received The previous list of represenations to receive from the other process.
   */
  void rollBack(const std::list<Provider>& providers);

public:
  /**
   * Constructor used when framework processes are mapped to threads.
   * In that case, each process sees all modules, because they are all declared in the
   * same address space. Therefore, this constructor filters them based on their categories.
   * @param categories The categories of modules executed by this process.
   */
  ModuleManager(const std::set<std::string>& categories);

  /**
   * Destructor.
   * Destructs all modules currently constructed.
   */
  ~ModuleManager();

  /**
   * The method loads the selection of solutions from a configuration file.
   */
  void load();

  /**
   * The method destroys all modules. It can be called to destroy the modules
   * before the constructor is called.
   */
  void destroy();

  /**
   * The method executes all selected modules.
   */
  void execute();
};
