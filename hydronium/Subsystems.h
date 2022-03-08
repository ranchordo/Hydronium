#ifndef SUBSYSTEMS
#define SUBSYSTEMS
#include "Arduino.h"
#include "PersistentMemoryManager.h"
#include "HydroniumUtil.h"
#include "uRTCLib.h"

#define PSTRUCT(name, inside) struct name ## PersistentData {inside ConfigFuncPtrManager< name > *cfpm = new ConfigFuncPtrManager< name >();}; const PROGMEM String name ## PersistentDataReflectionData= #inside ;
#define CONFIG(type,name) type name
#define CONFIGFUNC(ssname, name) ConfigurationFunctionPtr name = (uint32_t) & ## ssname ## :: ## name
#define MEMBER(type,name) type name
#define SUBSYSTEMBASE(name) name : public SubsystemBase<name ## PersistentData, &name ## PersistentDataReflectionData>
//typedef void ConfigurationFunctionRaw();
//typedef ConfigurationFunctionRaw *ConfigurationFunctionPtr;
typedef uint32_t ConfigurationFunctionPtr;

class SubsystemEntry {
  public:
  virtual void execute()=0;
  virtual uint64_t getNextTime(uRTCLib* rtc)=0;
  virtual String getName()=0;
  virtual void reflectPersistentDataStructure(ReflectableStructExtractor* extractor)=0;
  virtual uint32_t getPersistentDataStoreOffset()=0;
};

template<typename P, const String* reflectionData>
class SubsystemBase : public SubsystemEntry {
  public:
  SubsystemBase(String name) {this->name=name;}
  virtual void execute() override {}
  //Miliseconds since the start of the year
  virtual uint64_t getNextTime(uRTCLib* rtc) override {return ~((uint64_t)(0));}
  virtual void onPersistentPointerSet() {}
  void setPersistentDataStore(P* persistentData);
  uint32_t getPersistentDataStoreOffset() override {return (uint32_t)persistentData;}
  void reflectPersistentDataStructure(ReflectableStructExtractor* extractor) override;
  String getName() override {return name;}
  protected:
  String name;
  P* persistentData;
};
template<typename P, const String* reflectionData>
inline void SubsystemBase<P, reflectionData>::reflectPersistentDataStructure(ReflectableStructExtractor* extractor) {
  extractor->extractStructInformation(*reflectionData);
}
template<typename P, const String* reflectionData>
inline void SubsystemBase<P, reflectionData>::setPersistentDataStore(P* persistentData) {
  this->persistentData=persistentData;
}

template<typename S>
class ConfigFuncPtrManager {
  public:
  void callFuncPtr(ConfigurationFunctionPtr funcptr, SubsystemEntry* subsystem);
};

template<typename S>
inline void ConfigFuncPtrManager<S>::callFuncPtr(ConfigurationFunctionPtr funcptr32, SubsystemEntry* subsystem) {
  S* ss=(S*)(subsystem);
  void(S::*funcptr)() = (S::*)(funcptr32);
  ss->*funcptr();
}
#endif
