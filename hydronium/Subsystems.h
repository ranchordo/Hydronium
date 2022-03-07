#ifndef SUBSYSTEMS
#define SUBSYSTEMS
#include "Arduino.h"
#include "PersistentMemoryManager.h"
#include "HydroniumUtil.h"
#include "uRTCLib.h"

#define PSTRUCT(name, inside) struct name {inside}; const PROGMEM String name ## ReflectionData= #inside ;
#define CONFIG(type,name) type name
#define MEMBER(type,name) type name
#define SUBSYSTEMBASE(name) public SubsystemBase<name, &name ## ReflectionData>

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
#endif
