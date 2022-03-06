#ifndef INSTRUMENT
#define INSTRUMENT

#include "Arduino.h"
#include "PersistentMemoryManager.h"
#include "Subsystems.h"
#include "Interfaces.h"
#include "ConfigurationInterface.h"

struct InstrumentPersistentData {
  uint64_t nextTime=0;
  uint16_t subsystemID=0;
  bool notDoneFlag=false;
};

template<uint16_t numSubsystems=0>
class Instrument {
  public:
  Instrument(PersistentMemoryBlock* memoryBlock, InfoInterface* interface);
  void process();
  void initialize();
  ConfigurationInterface* getConfigurationInterface() {return &configInterface;}
  
  private:
  PersistentMemoryManager memoryManager;
  uint16_t index=0;
  SubsystemEntry* subsystems[numSubsystems];
  InstrumentPersistentData* persistentData;
  void getNextTimeToPersistentStruct();
  void setSecondLevelAlarm(uint64_t time); //time is in milliseconds since epoch
  void setAlarmMilliseconds(uint16_t ms);
  void deep_sleep_start();
  bool initialized=false;
  ConfigurationInterface configInterface;

  protected:
  InfoInterface* interface;
  virtual void addAllSubsystems()=0;
  template<typename P, const String* reflectionData> void addSubsystem(SubsystemBase<P, reflectionData>* subsystem);
};


#endif