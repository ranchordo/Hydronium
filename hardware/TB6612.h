#ifndef TB6612_H
#define TB6612_H
#include "Arduino.h"
#include "../hydronium/Sleepable.h"
#include "PinManager.h"

class TB6612: public Sleepable {
  private:
  uint8_t* stdby;
  uint8_t* pwm;
  uint8_t* ain1;
  uint8_t* ain2;
  uint8_t* bin1;
  uint8_t* bin2;
  public:
  TB6612(uint8_t* stdby, uint8_t* pwm, uint8_t* ain1, uint8_t* ain2, uint8_t* bin1, uint8_t* bin2);
  void setA(bool rn, bool ccw, uint8_t power); //Power=0,rn=1 should apply brake
  void setB(bool rn, bool ccw, uint8_t power); //Power=0,rn=1 should apply brake
  void wake() override;
  void sleep() override;
};

inline TB6612::TB6612(uint8_t* stdby, uint8_t* pwm, uint8_t* ain1, uint8_t* ain2, uint8_t* bin1, uint8_t* bin2) {
  this->stdby=stdby;
  this->pwm=pwm;
  this->ain1=ain1;
  this->ain2=ain2;
  this->bin1=bin1;
  this->bin2=bin2;
}
inline void TB6612::setA(bool rn, bool ccw, uint8_t power) {
  if(ccw) {
    PinManager::dwrite(*ain1,rn&&(!ccw));
  }
  PinManager::dwrite(*ain2,rn&&ccw);
  if(!ccw) {
    PinManager::dwrite(*ain1,rn&&(!ccw));
  }
  PinManager::awrite(*pwm,rn?power:0);
}
inline void TB6612::setB(bool rn, bool ccw, uint8_t power) {
  if(ccw) {
    PinManager::dwrite(*bin1,rn&&(!ccw));
  }
  PinManager::dwrite(*bin2,rn&&ccw);
  if(!ccw) {
    PinManager::dwrite(*bin1,rn&&(!ccw));
  }
  PinManager::awrite(*pwm,rn?power:0);
}
inline void TB6612::sleep() {
  this->setA(false,false,0);
  this->setB(false,false,0);
  PinManager::dwrite(*stdby,LOW);
}
inline void TB6612::wake() {
  PinManager::dwrite(*stdby,HIGH);
}
#endif