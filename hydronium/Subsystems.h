#ifndef SUBSYSTEMS
#define SUBSYSTEMS
#include "Arduino.h"
#include "PersistentMemoryManager.h"

class SubsystemEntry {
  public:
  virtual void execute()=0;
  virtual uint64_t getNextTime()=0;
};

template<typename P>
class SubsystemBase : public SubsystemEntry {
  public:
  virtual void execute() override {}
  //Miliseconds since the start of the year
  virtual uint64_t getNextTime() override {return ~((uint64_t)(0));}
  virtual void onPersistentPointerSet() {}
  void setPersistentDataStore(P* persistentData);
  protected:
  P* persistentData;
};

struct InstrumentPersistentData {
  uint64_t nextTime=0;
  uint16_t subsystemID=0;
};

template<int numSubsystems=0>
class Instrument {
  public:
  Instrument(PersistentMemoryBlock* memoryBlock);
  void process();
  void initialize();
  
  private:
  PersistentMemoryManager memoryManager;
  uint16_t index=0;
  SubsystemEntry* subsystems[numSubsystems];
  InstrumentPersistentData* persistentData;
  void getNextTimeToPersistentStruct();

  protected:
  virtual void addAllSubsystems()=0;
  template<typename P>
  void addSubsystem(SubsystemBase<P>* subsystem);
};
#endif
