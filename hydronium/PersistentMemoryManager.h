#ifndef PERSISTENTMEMORYMANAGER_IMPL
#define PERSISTENTMEMORYMANAGER_IMPL
#include "PersistentMemoryManager_defs.h"

RTC_DATA_ATTR union PersistentMemoryBlock persistentMemoryBlock;

inline PersistentMemoryManager::PersistentMemoryManager(PersistentMemoryBlock* memoryBlock) {
  this->setMemoryBlock(memoryBlock);
  this->resetAssignment();
}
inline void PersistentMemoryManager::setMemoryBlock(PersistentMemoryBlock* memoryBlock) {
  this->memoryBlock=memoryBlock;
}
inline void PersistentMemoryManager::resetAssignment() {
  this->index=(uint32_t)(&this->memoryBlock->persistentMemory.persistentMemoryStart);
}
inline uint32_t PersistentMemoryManager::getNextAvailableAddress() {
  return this->index;
}
inline uint32_t PersistentMemoryManager::addPersistentBlock(uint32_t len) {
  this->index+=len;
  if(this->index>((uint32_t)(((PersistentMemoryBlock*)0)+1))) {
    //TODO: Add logger message for a persistent memory overflow
  }
  return this->index-len;
}
inline PersistentMemoryBlock* PersistentMemoryManager::getMemoryBlock() {
  return this->memoryBlock;
}
#endif