#include "PersistentMemoryManager.h"
RTC_DATA_ATTR union PersistentMemoryBlock persistentMemoryBlock;

PersistentMemoryManager::PersistentMemoryManager(PersistentMemoryBlock* memoryBlock) {
  this->setMemoryBlock(memoryBlock);
  this->resetAssignment();
}
void PersistentMemoryManager::setMemoryBlock(PersistentMemoryBlock* memoryBlock) {
  this->memoryBlock=memoryBlock;
}
void PersistentMemoryManager::resetAssignment() {
  this->index=(uint32_t)(&this->memoryBlock->persistentMemory.persistentMemoryStart);
}
uint32_t PersistentMemoryManager::getNextAvailableAddress() {
  return this->index;
}
uint32_t PersistentMemoryManager::addPersistentBlock(uint32_t len) {
  this->index+=len;
  if(this->index>((uint32_t)(((PersistentMemoryBlock*)0)+1))) {
    //TODO: Add logger message for a persistent memory overflow
  }
  return this->index-len;
}
PersistentMemoryBlock* PersistentMemoryManager::getMemoryBlock() {
  return this->memoryBlock;
}
