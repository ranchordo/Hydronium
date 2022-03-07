#ifndef INSTRUMENT_IMPL
#define INSTRUMENT_IMPL

#include "Instrument_defs.h"
//Inline function declarations to dodge linker errors


template<uint16_t numSubsystems>
template<typename P, const String* reflectionData>
inline void Instrument<numSubsystems>::addSubsystem(SubsystemBase<P, reflectionData>* subsystem) {
  #ifdef PRINT_DEBUG_INFO
  interface->log(0,"Adding subsystem \""+subsystem->getName()+"\"");
  #endif
  if(index<numSubsystems) {
    //Intiailize persistent data store for this subsystem
    P* zeropointer=0;
    zeropointer++;
    P* persistentPointer=(P*)(this->memoryManager.addPersistentBlock((uint32_t)zeropointer));
    P initials;
    persistentPointer[0]=initials;
    subsystem->setPersistentDataStore(persistentPointer);
    subsystem->onPersistentPointerSet();

    //Add to subsystems list
    this->subsystems[this->index]=subsystem;
    this->index++;
  } else {
    //TODO: Add logger error message. We've initialized too many subsystems.
  }
}
template<uint16_t numSubsystems>
inline Instrument<numSubsystems>::Instrument(PersistentMemoryBlock* memoryBlock, InfoInterface* interface, uRTCLib* rtc) : memoryManager(memoryBlock), configInterface(interface, subsystems, numSubsystems) {
  this->persistentData=0;
  this->interface=interface;
  this->persistentData=(InstrumentPersistentData*)memoryManager.addPersistentBlock((uint32_t)(this->persistentData+1));
  this->rtc=rtc;
}
template<uint16_t numSubsystems>
inline void Instrument<numSubsystems>::initialize() {
  interface->begin();
  #ifdef PRINT_DEBUG_INFO
  interface->log(0,"Initializing instrument..."); //Use lowercase serial here as a pointer, it's specific to this instrument
  #endif
  this->addAllSubsystems();
}
template<uint16_t numSubsystems>
inline void Instrument<numSubsystems>::getNextTimeToPersistentStruct() {
  persistentData->nextTime=~((uint64_t)(0)); //Largest possible value
  for(int i=0;i<min(this->index,numSubsystems);i++) {
    uint64_t nextTime=this->subsystems[i]->getNextTime(this->rtc);
    if(nextTime < this->persistentData->nextTime) {
      this->persistentData->nextTime=nextTime;
      this->persistentData->subsystemID=i;
    }
  }
}
template<uint16_t numSubsystems>
inline void Instrument<numSubsystems>::process() {
  if(!this->initialized) {this->initialize();}
  if(this->persistentData->notDoneFlag) {
    this->persistentData->notDoneFlag=false;
    this->setAlarmMilliseconds(this->persistentData->nextTime % ((uint64_t)1000));
    this->deep_sleep_start();
  } else {
    this->subsystems[this->persistentData->subsystemID]->execute();
    this->getNextTimeToPersistentStruct();
    if((this->persistentData->nextTime % ((uint64_t)1000))>5) { //If we're not within x milliseconds of the actual alarm
      this->persistentData->notDoneFlag=true;
    }
    this->setSecondLevelAlarm(this->persistentData->nextTime);
    this->deep_sleep_start();
  }
  //If we get here.... I don't even know anymore
  //TODO: Add logger message for insanity
}

template<uint16_t numSubsystems>
inline void Instrument<numSubsystems>::deep_sleep_start() {
  esp_deep_sleep_start();
}

template<uint16_t numSubsystems>
inline void Instrument<numSubsystems>::setSecondLevelAlarm(uint64_t time) {
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);
  //TODO: Fix
}
template<uint16_t numSubsystems>
inline void Instrument<numSubsystems>::setAlarmMilliseconds(uint16_t ms) {
   esp_sleep_enable_timer_wakeup(ms*1000);
}
#endif