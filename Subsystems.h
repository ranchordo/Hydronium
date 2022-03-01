#ifndef SUBSYSTEMS
#define SUBSYSTEMS
#include "Arduino.h"
#include "PersistentMemoryManager.h"

class SubsystemEntry {
  public:
  virtual void execute()=0;
};

template<typename P>
class SubsystemBase : public SubsystemEntry {
  public:
  virtual void execute() override {}
  virtual void onPersistentPointerSet() {}
  void setPersistentDataStore(P* persistentData);
  protected:
  P* persistentData;
};

template<int numSubsystems=0>
class Instrument {
  public:
  Instrument(PersistentMemoryBlock* memoryBlock);
  void process();
  
  private:
  PersistentMemoryManager memoryManager;
  uint16_t index=0;
  SubsystemEntry* subsystems[numSubsystems];

  protected:
  virtual void addAllSubsystems()=0;
  template<typename P>
  void addSubsystem(SubsystemBase<P>* subsystem);
};
#endif
