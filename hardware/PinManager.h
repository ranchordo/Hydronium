#ifndef PINMANAGER_H
#define PINMANAGER_H
class PinManager {
  public:
  static void dwrite(byte pin, byte state);
  static bool dread(byte pin);
  static void awrite(byte pin, byte power);
  static void init();
  static uint16_t getPWMModes() {return PWMModes;}
  static const uint8_t IOs[19];
  private:
  static uint16_t PWMModes;
};
uint16_t PinManager::PWMModes=0;
const uint8_t PinManager::IOs[19] {3,1,25,26,27,9,10,13,5,2,6,7,8,11,17,16,4,12,14};

inline void PinManager::dwrite(byte p, byte state) {
  pinMode(IOs[p],OUTPUT);
  if(PWMModes & (1<<p)) {
    ledcDetachPin(p);
    PWMModes &= ~(1<<p);
  }
  #ifdef PRINT_DBG_INFO_PINMANAGER
  Serial.println("dwrite: "+String(p)+" "+String(state));
  #endif
  digitalWrite(IOs[p], state);
}
inline bool PinManager::dread(byte p) {
  pinMode(IOs[p],INPUT);
  if(PWMModes & (1<<p)) {
    ledcDetachPin(p);
    PWMModes &= ~(1<<p);
  }
  return digitalRead(p);
}
inline void PinManager::awrite(byte p, byte power) {
  pinMode(IOs[p],OUTPUT);
  if(!(PWMModes & (1<<p))) {
    #ifdef PRINT_DBG_INFO_PINMANAGER
    Serial.println("PWMINIT "+String(p));
    #endif
    ledcAttachPin(IOs[p],p);
    ledcSetup(p,4000,8);
    PWMModes |= 1<<p;
  }
  #ifdef PRINT_DBG_INFO_PINMANAGER
  Serial.println("awrite: "+String(p)+" "+String(power));
  #endif
  ledcWrite(p,power);
}
#endif