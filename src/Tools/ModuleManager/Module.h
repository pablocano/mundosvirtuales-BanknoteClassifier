/**
 * @file Module.h
 * Definition of the module handling schema.
 *
 * This file provides the ability to specify the requirements of and the
 * representations provided by a module. Example:
 *
 * MODULE(MyImageProcessor,
 * {,
 *   REQUIRES(Image),                               // Has to be updated before
 *   REQUIRES(CameraMatrix),                        // Has to be updated before
 *   USES(RobotPose),                               // Is used, but has not to be updated before
 *   PROVIDES_WITH_MODIFY(BallPercept),             // Class provides a method to update BallPercept. Representation can be MODIFYed.
 *   PROVIDES_WITH_OUTPUT(BeaconsPercept),          // Class provides a method to update BeaconsPercept. Representation can be requested.
 *   PROVIDES_WITH_MODIFY_AND_OUTPUT(LinesPercept), // Class provides a method to update LinesPercept. Representation can be MODIFYed and requested to be sent with given message.
 *   PROVIDES(GoalPercept),                         // Class provides a method to update GoalsPercept.
 *   DEFINES_PARAMETERS(                            // Has parameters that must have an initial value. If LOADS_PARAMETERS is used instead,
 *   {,                                             // they are loaded from a configuration file that has the same name as the module defined, but starts with lowercase letters.
 *     (float)(42.0f) focalLen,                     // Has a parameter named focalLen of type float. By default it has the value 42.0f.
 *     (int)(21) resWidth,                          // All attributes are streamed and can be accessed by requesting 'parameters:MyImageProcessor'
 *     (CameraInfo, Camera)(camera) lower,          // Use an enum as parameter that was defined in another class.
 *   }),
 * });
 *
 * This block defines a base class MyImageProcessorBase. The module MyImageProcessor has to be
 * derived from that class:
 *
 * class MyImageProcessor : public MyImageProcessorBase
 * {
 *   void update(BallPercept& ballPercept);
 *   void update(BeaconsPercept& beaconsPercept);
 *   void update(GoalsPercept& goalsPercept);
 * };
 *
 * In the implementation file, the existence of the module has to be announced:
 *
 * MAKE_MODULE(MyImageProcessor, Perception)
 *
 * The second parameter defines a category that is used to group modules.
 *
 * @author Thomas Röfer
 */

#pragma once

#include "Tools/File.h"
#include "Tools/Streamable.h"
#include "Blackboard.h"

/**
 * The class is the abstract base of all template classes that create modules.
 */
class ModuleBase
{
public:
    /** Helpers to check whether a class defines a draw method that is not inherited. */
    template<typename T, void (T::*)()> struct Draw {};
    template<typename T, void (T::*)() const> struct ConstDraw {};

    /**
     * Calls a draw method if a representation has one that is not inherited.
     * @param T The type of the representation.
     * @param t The representation.
     */
    template<typename T> static auto draw(T* t) -> decltype(Draw<T, &T::draw>(), void()) {t->draw();}
    template<typename T> static auto draw(const T* t) -> decltype(ConstDraw<T, &T::draw>(), void()) {t->draw();}
    static void draw(void*) {}
    static void draw(const void*) {}
  class Info
  {
  public:
    const char* representation;
    void (*update)(Streamable&);
    
    Info(const char* representation, void (*update)(Streamable&))
    : representation(representation), update(update) {}
  };
  
private:
  static ModuleBase* first; /**< The head of the list of all modules available. */
  ModuleBase* next; /**< The next entry in the list of all modules. */
  const char* name; /**< The name of the module that can be created by this instance. */
  const char* category; /**< The name of the category of this module. */
  const Info* info; /**< Information about the requirements and provisions of the module. */
  
protected:
  /**
   * Abstract method to create an instance of a module.
   * @return The address of the instance created.
   */
  virtual Streamable* createNew() = 0;
  
public:
  /**
   * Constructor.
   * @param name The name of the module that can be created by this instance.
   * @param category The name of the category of this module.
   */
  ModuleBase(const char* name, const char* category, const Info* info)
  : next(first),
  name(name),
  category(category),
  info(info)
  {
    first = this;
  }
  
  friend class ModuleManager; /**< The ModuleManager gathers all private data. */
};

/**
 * @class Module
 * The template class provides a method to create a certain module, and it
 * registers all requirements and representations provided by that class.
 * @param M The type of the module created.
 * @param B The base class of the module.
 */
template<class M, class B> class Module : public ModuleBase
{
private:
  /**
   * The method creates an instance of the module.
   * @return The address of the newly created instance.
   */
  Streamable* createNew()
  {
    return (Streamable*) new M;
  }
  
public:
  /**
   * Constructor.
   * The constructor registers all requirements and representations provided
   * of the module that can be created by this instance.
   * @attention Since the module itself should not be created to just register
   * its requirements and representations, because the constructor of the module
   * may do time-consuming operations, the base class has to be used directly
   * to perform the registration. However, the base class is abstract, so it
   * cannot be constructed. To overcome this limitation, an assignment is faked,
   * and it is used to do the registration of the information required.
   * @param name The name of the module that can be created by this instance.
   * @param category The name of the category of this module.
   */
  Module(const char* name, const char* category)
  : ModuleBase(name, category, B::getModuleInfo()) {}
};

// Some of the following macros can also be found in AutoStreamable.h with different names.
// However, separate versions are required here, because the preprocessor only expands each
// macro once in a recursive structure.

/**
 * Determine the number of entries in a tuple.
 */
#ifdef WINDOWS
#define _MODULE_TUPLE_SIZE(...) _MODULE_JOIN(_MODULE_TUPLE_SIZE_II, (__VA_ARGS__, \
100, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, \
79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, \
59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1))
#else
#define _MODULE_TUPLE_SIZE(...) _MODULE_TUPLE_SIZE_I((__VA_ARGS__, \
100, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, \
79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, \
59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1))
#define _MODULE_TUPLE_SIZE_I(params) _MODULE_TUPLE_SIZE_II params
#endif

/**
 * The last part of a macro to determine the number of entries in a tuple.
 */
#define _MODULE_TUPLE_SIZE_II( \
a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, \
a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, \
a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, \
a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, \
a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, ...) a100

/**
 * Apply a macro to all elements of a tuple.
 */
#define _MODULE_ATTR_1(f, a1)
#define _MODULE_ATTR_2(f, a1, a2) f(a1)
#define _MODULE_ATTR_3(f, a1, a2, a3) f(a1) f(a2)
#define _MODULE_ATTR_4(f, a1, a2, a3, a4) f(a1) f(a2) f(a3)
#define _MODULE_ATTR_5(f, a1, a2, a3, a4, a5) f(a1) f(a2) f(a3) f(a4)
#define _MODULE_ATTR_6(f, a1, a2, a3, a4, a5, a6) f(a1) f(a2) f(a3) f(a4) f(a5)
#define _MODULE_ATTR_7(f, a1, a2, a3, a4, a5, a6, a7) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6)
#define _MODULE_ATTR_8(f, a1, a2, a3, a4, a5, a6, a7, a8) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7)
#define _MODULE_ATTR_9(f, a1, a2, a3, a4, a5, a6, a7, a8, a9) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8)
#define _MODULE_ATTR_10(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9)
#define _MODULE_ATTR_11(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10)
#define _MODULE_ATTR_12(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11)
#define _MODULE_ATTR_13(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12)
#define _MODULE_ATTR_14(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13)
#define _MODULE_ATTR_15(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14)
#define _MODULE_ATTR_16(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15)
#define _MODULE_ATTR_17(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16)
#define _MODULE_ATTR_18(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17)
#define _MODULE_ATTR_19(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18)
#define _MODULE_ATTR_20(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19)
#define _MODULE_ATTR_21(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20)
#define _MODULE_ATTR_22(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21)
#define _MODULE_ATTR_23(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22)
#define _MODULE_ATTR_24(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23)
#define _MODULE_ATTR_25(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24)
#define _MODULE_ATTR_26(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25)
#define _MODULE_ATTR_27(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26)
#define _MODULE_ATTR_28(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27)
#define _MODULE_ATTR_29(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28)
#define _MODULE_ATTR_30(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29)
#define _MODULE_ATTR_31(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30)
#define _MODULE_ATTR_32(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31)
#define _MODULE_ATTR_33(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32)
#define _MODULE_ATTR_34(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33)
#define _MODULE_ATTR_35(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34)
#define _MODULE_ATTR_36(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35)
#define _MODULE_ATTR_37(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36)
#define _MODULE_ATTR_38(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37)
#define _MODULE_ATTR_39(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38)
#define _MODULE_ATTR_40(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39)
#define _MODULE_ATTR_41(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40)
#define _MODULE_ATTR_42(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41)
#define _MODULE_ATTR_43(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42)
#define _MODULE_ATTR_44(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43)
#define _MODULE_ATTR_45(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44)
#define _MODULE_ATTR_46(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45)
#define _MODULE_ATTR_47(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45) f(a46)
#define _MODULE_ATTR_48(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45) f(a46) f(a47)
#define _MODULE_ATTR_49(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45) f(a46) f(a47) f(a48)
#define _MODULE_ATTR_50(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45) f(a46) f(a47) f(a48) f(a49)
#define _MODULE_ATTR_51(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45) f(a46) f(a47) f(a48) f(a49) f(a50)
#define _MODULE_ATTR_52(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45) f(a46) f(a47) f(a48) f(a49) f(a50) f(a51)
#define _MODULE_ATTR_53(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45) f(a46) f(a47) f(a48) f(a49) f(a50) f(a51) f(a52)
#define _MODULE_ATTR_54(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45) f(a46) f(a47) f(a48) f(a49) f(a50) f(a51) f(a52) f(a53)
#define _MODULE_ATTR_55(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45) f(a46) f(a47) f(a48) f(a49) f(a50) f(a51) f(a52) f(a53) f(a54)
#define _MODULE_ATTR_56(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45) f(a46) f(a47) f(a48) f(a49) f(a50) f(a51) f(a52) f(a53) f(a54) f(a55)
#define _MODULE_ATTR_57(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45) f(a46) f(a47) f(a48) f(a49) f(a50) f(a51) f(a52) f(a53) f(a54) f(a55) f(a56)
#define _MODULE_ATTR_58(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45) f(a46) f(a47) f(a48) f(a49) f(a50) f(a51) f(a52) f(a53) f(a54) f(a55) f(a56) f(a57)
#define _MODULE_ATTR_59(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45) f(a46) f(a47) f(a48) f(a49) f(a50) f(a51) f(a52) f(a53) f(a54) f(a55) f(a56) f(a57) f(a58)
#define _MODULE_ATTR_60(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45) f(a46) f(a47) f(a48) f(a49) f(a50) f(a51) f(a52) f(a53) f(a54) f(a55) f(a56) f(a57) f(a58) f(a59)
#define _MODULE_ATTR_61(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45) f(a46) f(a47) f(a48) f(a49) f(a50) f(a51) f(a52) f(a53) f(a54) f(a55) f(a56) f(a57) f(a58) f(a59) f(a60)
#define _MODULE_ATTR_62(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45) f(a46) f(a47) f(a48) f(a49) f(a50) f(a51) f(a52) f(a53) f(a54) f(a55) f(a56) f(a57) f(a58) f(a59) f(a60) f(a61)
#define _MODULE_ATTR_63(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45) f(a46) f(a47) f(a48) f(a49) f(a50) f(a51) f(a52) f(a53) f(a54) f(a55) f(a56) f(a57) f(a58) f(a59) f(a60) f(a61) f(a62)
#define _MODULE_ATTR_64(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45) f(a46) f(a47) f(a48) f(a49) f(a50) f(a51) f(a52) f(a53) f(a54) f(a55) f(a56) f(a57) f(a58) f(a59) f(a60) f(a61) f(a62) f(a63)
#define _MODULE_ATTR_65(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45) f(a46) f(a47) f(a48) f(a49) f(a50) f(a51) f(a52) f(a53) f(a54) f(a55) f(a56) f(a57) f(a58) f(a59) f(a60) f(a61) f(a62) f(a63) f(a64)
#define _MODULE_ATTR_66(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45) f(a46) f(a47) f(a48) f(a49) f(a50) f(a51) f(a52) f(a53) f(a54) f(a55) f(a56) f(a57) f(a58) f(a59) f(a60) f(a61) f(a62) f(a63) f(a64) f(a65)
#define _MODULE_ATTR_67(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45) f(a46) f(a47) f(a48) f(a49) f(a50) f(a51) f(a52) f(a53) f(a54) f(a55) f(a56) f(a57) f(a58) f(a59) f(a60) f(a61) f(a62) f(a63) f(a64) f(a65) f(a66)
#define _MODULE_ATTR_68(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45) f(a46) f(a47) f(a48) f(a49) f(a50) f(a51) f(a52) f(a53) f(a54) f(a55) f(a56) f(a57) f(a58) f(a59) f(a60) f(a61) f(a62) f(a63) f(a64) f(a65) f(a66) f(a67)
#define _MODULE_ATTR_69(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45) f(a46) f(a47) f(a48) f(a49) f(a50) f(a51) f(a52) f(a53) f(a54) f(a55) f(a56) f(a57) f(a58) f(a59) f(a60) f(a61) f(a62) f(a63) f(a64) f(a65) f(a66) f(a67) f(a68)
#define _MODULE_ATTR_70(f, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70) f(a1) f(a2) f(a3) f(a4) f(a5) f(a6) f(a7) f(a8) f(a9) f(a10) f(a11) f(a12) f(a13) f(a14) f(a15) f(a16) f(a17) f(a18) f(a19) f(a20) f(a21) f(a22) f(a23) f(a24) f(a25) f(a26) f(a27) f(a28) f(a29) f(a30) f(a31) f(a32) f(a33) f(a34) f(a35) f(a36) f(a37) f(a38) f(a39) f(a40) f(a41) f(a42) f(a43) f(a44) f(a45) f(a46) f(a47) f(a48) f(a49) f(a50) f(a51) f(a52) f(a53) f(a54) f(a55) f(a56) f(a57) f(a58) f(a59) f(a60) f(a61) f(a62) f(a63) f(a64) f(a65) f(a66) f(a67) f(a68) f(a69)

/** Concatenate the two parameters. */
#define _MODULE_JOIN(a, b) _MODULE_JOIN_I(a, b)
#define _MODULE_JOIN_I(a, b) _MODULE_JOIN_II(a ## b)
#define _MODULE_JOIN_II(res) res

/**
 * The following macros generate the declarations for all requirements as well as
 * the update handler for the providers.code for loading the configuration file
 * from LOADS_PARAMETERS. They filter out the *_PARAMETERS macros.
 * @param x The type name of a representation or the set of all parameters.
 */
#define _MODULE_DECLARE(x) _MODULE_JOIN(_MODULE_DECLARE_, x)
#define _MODULE_DECLARE_PROVIDES(type) _MODULE_PROVIDES(type, \
  ModuleBase::draw(&r); )
#define _MODULE_DECLARE_REQUIRES(type) public: const type& the##type = Blackboard::getInstance().alloc<type>(#type);
#define _MODULE_DECLARE_USES(type) public: const type& the##type = Blackboard::getInstance().alloc<type>(#type);

/**
 * The following macros generate the code that frees all requirements
 * and providers. They filter out the *_PARAMETERS macros.
 * @param x The type name of a representation or the set of all parameters.
 */
#define _MODULE_FREE(x) _MODULE_JOIN(_MODULE_FREE_, x)
#define _MODULE_FREE_PROVIDES(type) if(_the##type) Blackboard::getInstance().free(#type);
#define _MODULE_FREE_REQUIRES(type) Blackboard::getInstance().free(#type);
#define _MODULE_FREE_USES(type) Blackboard::getInstance().free(#type);

/**
 * The following macros generate the code that provides information about all requirements
 * and providers. They filter out the *_PARAMETERS and USES macros.
 * @param x The type name of a representation or the set of all parameters.
 */
#define _MODULE_INFO(x) _MODULE_JOIN(_MODULE_INFO_, x)
#define _MODULE_INFO_PROVIDES(type) ModuleBase::Info(#type, &BaseType::update##type),
#define _MODULE_INFO_REQUIRES(type) ModuleBase::Info(#type, 0),
#define _MODULE_INFO_USES(type)

/**
 * The macro defines the code added for each PROVIDES.
 * It declares the abstract update method, a pointer to the representation provided,
 * and an static handler that calls the update method.
 * @param type The type of the representation provided.
 * @param mod Additional code that is added to the handler.
 */
#define _MODULE_PROVIDES(type, mod) \
protected: virtual void update(type&) = 0; \
\
private: type* _the##type = 0; \
static void update##type(Streamable& module) \
{ \
  if(!((BaseType&) module)._the##type) \
    ((BaseType&) module)._the##type = &Blackboard::getInstance().alloc<type>(#type); \
  type& r(*((BaseType&) module)._the##type); \
  ((BaseType&) module).update(r); \
  mod \
}

/**
 * Helper for defining the module's base class.
 * @param name The name of the module.
 * @param n The number of entries in the third parameter.
 * @param ... The requirementes, provided representations and parameter definitions.
 */
#define _MODULE_I(name, n, ...) _MODULE_II(name, n, (_MODULE_DECLARE, __VA_ARGS__), (_MODULE_FREE, __VA_ARGS__), (_MODULE_INFO, __VA_ARGS__))

/**
 * Generates the actual code of the module's base class.
 * It create all the code and fills in data from the requirements, representations,
 * provided, and parameters defined.
 */
#define _MODULE_II(name, n, declare, free, info) \
class name; \
class name##Base : public Streamable \
{ \
private: \
  typedef name##Base BaseType; \
private: \
  static const ModuleBase::Info* getModuleInfo() \
  { \
    static const ModuleBase::Info infos[] = \
    { \
      _MODULE_ATTR_##n info \
      ModuleBase::Info(0, 0) \
    }; \
    return infos; \
  } \
  friend class Module<name, name##Base>; \
  _MODULE_ATTR_##n declare \
  public: \
  name##Base(const char* fileName = 0) \
  { \
    static const char* moduleName = #name; \
    (void) moduleName; \
  } \
  ~name##Base() \
  { \
    _MODULE_ATTR_##n free \
  } \
  name##Base(const name##Base&) = delete; \
  name##Base& operator=(const name##Base&) = delete; \
}

/**
 * Generate the module's base class from the MODULE description.
 * See beginning of this file.
 * @param name The name of the module.
 * @param header Normally, only an opening brace.
 * @param ... The requirementes, provided representations and parameter definitions.
 */
#define MODULE(name, header, ...) \
_MODULE_I(name, _MODULE_TUPLE_SIZE(__VA_ARGS__), __VA_ARGS__)

/**
 * The macro creates a creator for the module.
 * See beginning of this file.
 * It has to be part of the implementation file.
 * @param module The name of the module that can be created.
 * @param category The name of the category of this module.
 */
#define MAKE_MODULE(module, category) \
Module<module, module##Base> the##module##Module(#module, #category);

