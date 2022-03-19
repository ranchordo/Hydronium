#ifndef SUBSYSTEMS
#define SUBSYSTEMS
#include "Arduino.h"
#include "PersistentMemoryManager.h"
#include "HydroniumUtil.h"
#include "uRTCLib.h"
#include "Sleepable.h"

#define MAX_SLEEPABLES_PER_SUBSYSTEM 16

#define PSTRUCT(name,inside) struct name ## PersistentData {inside uint32_t cfpm_ptr; static const bool isValidPersistentStruct=true;}; const PROGMEM String name ## PersistentDataReflectionData= #inside ;
#define CONFIG(type,name) type name
#define MEMBER(type,name) type name
#define SUBSYSTEMBASE(name) name :public SubsystemBase<name ## PersistentData,&name ## PersistentDataReflectionData>
#define INIT_CONFIG_FUNCS(ssname) this->persistentData->cfpm_ptr=(uint32_t)((ConfigFuncPtrManagerGeneric*)(new ConfigFuncPtrManager< ssname >()))
#define CREATE_CONFIG_FUNC(ssname,name) this->persistentData->name=functionPtr(&ssname::name)
//typedef void ConfigurationFunctionRaw();
//typedef ConfigurationFunctionRaw *ConfigurationFunctionPtr;

class ConfigFuncPtrManagerGeneric {
  public:
  virtual void callFuncPtr(ConfigurationFunctionPtr intfuncptr, uint32_t subsystemEntry)=0;
};

class SubsystemEntry {
  public:
  virtual void execute()=0;
  virtual uint64_t getNextTime(uRTCLib* rtc)=0;
  virtual String getName()=0;
  virtual void reflectPersistentDataStructure(ReflectableStructExtractor* extractor)=0;
  virtual uint32_t getPersistentDataStoreOffset()=0;
  virtual ConfigFuncPtrManagerGeneric* getCFPM()=0;
  virtual void sleepSubsystem()=0;
  virtual void wakeSubsystem()=0;
};

template<typename P, const String* reflectionData>
class SubsystemBase : public SubsystemEntry {
  static_assert(P::isValidPersistentStruct, "P must inherit from PersistentData or have a field of cfpm_ptr.\
  Make sure you have initialized this struct with PTSTRUCT.");
  public:
  SubsystemBase(String name) {this->name=name;}
  virtual void execute() override {}
  //Miliseconds since the start of the year
  virtual uint64_t getNextTime(uRTCLib* rtc) override {return ~((uint64_t)(0));}
  virtual void onPersistentPointerSet() {}
  void setPersistentDataStore(P* persistentData);
  uint32_t getPersistentDataStoreOffset() override {return (uint32_t)persistentData;}
  ConfigFuncPtrManagerGeneric* getCFPM() override {return (ConfigFuncPtrManagerGeneric*)(this->persistentData->cfpm_ptr);}
  void reflectPersistentDataStructure(ReflectableStructExtractor* extractor) override;
  String getName() override {return name;}
  void setInterface(InfoInterface* i) {this->interface=i;}
  void registerSleepable(Sleepable* sleepable);
  void sleepSubsystem() override;
  void wakeSubsystem() override;
  protected:
  String name;
  P* persistentData;
  Sleepable *sleepables[MAX_SLEEPABLES_PER_SUBSYSTEM];
  uint8_t sleepableIndex=0;
  InfoInterface* interface;
};
template<typename P, const String* reflectionData>
inline void SubsystemBase<P, reflectionData>::registerSleepable(Sleepable* sleepable) {
  sleepables[sleepableIndex]=sleepable;
  sleepableIndex++;
}
template<typename P, const String* reflectionData>
inline void SubsystemBase<P, reflectionData>::sleepSubsystem() {
  for(int i=0;i<this->sleepableIndex;i++) {
    sleepables[i]->sleep();
  }
}
template<typename P, const String* reflectionData>
inline void SubsystemBase<P, reflectionData>::wakeSubsystem() {
  for(int i=0;i<this->sleepableIndex;i++) {
    sleepables[i]->wake();
  }
}
template<typename P, const String* reflectionData>
inline void SubsystemBase<P, reflectionData>::reflectPersistentDataStructure(ReflectableStructExtractor* extractor) {
  extractor->extractStructInformation(*reflectionData);
}
template<typename P, const String* reflectionData>
inline void SubsystemBase<P, reflectionData>::setPersistentDataStore(P* persistentData) {
  this->persistentData=persistentData;
}
template<typename S>
class ConfigFuncPtrManager : public ConfigFuncPtrManagerGeneric {
  public:
  void callFuncPtr(ConfigurationFunctionPtr intfuncptr, uint32_t subsystemEntry) override;
};

template<typename S>
inline void ConfigFuncPtrManager<S>::callFuncPtr(ConfigurationFunctionPtr intfuncptr, uint32_t subsystemEntry) {
  S* ss=(S*)((SubsystemEntry*)subsystemEntry);
  void(S::*funcptr)() = *reinterpret_cast<void(S::**)()>(&intfuncptr);
  (ss->*funcptr)();
}
#endif
