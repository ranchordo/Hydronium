#include "Subsystems.h"

template<typename P>
void SubsystemBase<P>::setPersistentDataStore(P* persistentData) {
  this->persistentData=persistentData;
}

template<int numSubsystems>
Instrument<numSubsystems>::Instrument(PersistentMemoryBlock* memoryBlock) : memoryManager(memoryBlock) {
  this->persistentData=0;
  this->persistentData=(InstrumentPersistentData*)memoryManager.addPersistentBlock((uint32_t)(this->persistentData+1));
}
template<int numSubsystems>
template<typename P>
void Instrument<numSubsystems>::addSubsystem(SubsystemBase<P>* subsystem) {
  if(index<numSubsystems) {
    //Intiailize persistent data store for this subsystem
    P* zeropointer=0;
    zeropointer++;
    P* persistentPointer=(P*)(this->memoryManager.addPersistentBlock((uint32_t)zeropointer));
    subsystem->setPersistentDataStore(persistentPointer);
    subsystem->onPersistentPointerSet();

    //Add to subsystems list
    this->subsystems[this->index]=subsystem;
    this->index++;
  } else {
    //TODO: Add logger error message. We've initialized too many subsystems.
  }
}
template<int numSubsystems>
void Instrument<numSubsystems>::initialize() {
  this->addAllSubsystems();
}
template<int numSubsystems>
void Instrument<numSubsystems>::getNextTimeToPersistentStruct() {
  persistentData->nextTime=~((uint64_t)(0)); //Largest possible value
  for(int i=0;i<this->min(index,numSubsystems);i++) {
    uint64_t nextTime=this->subsystems[i]->getNextTime();
    if(nextTime < this->persistentData->nextTime) {
      this->persistentData->nextTime=nextTime;
      this->persistentData->subsystemID=i;
    }
  }
}
