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

template<typename P>
inline void SubsystemBase<P>::setPersistentDataStore(P* persistentData) {
  this->persistentData=persistentData;
}
#endif
