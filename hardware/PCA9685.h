//Stolen from https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library/blob/master/Adafruit_PWMServoDriver.cpp
#ifndef PCA9685_IMPL
#define PCA9685_IMPL
#include "PCA9685_defs.h"
#include <Wire.h>

inline PCA9685::PCA9685()
    : _i2caddr(PCA9685_I2C_ADDRESS), _i2c(&Wire) {}

inline PCA9685::PCA9685(const uint8_t addr)
    : _i2caddr(addr), _i2c(&Wire) {}

inline PCA9685::PCA9685(const uint8_t addr,
                                                 TwoWire &i2c)
    : _i2caddr(addr), _i2c(&i2c) {}

inline void PCA9685::begin(uint8_t prescale) {
  //_i2c->begin();
  reset();
  if (prescale) {
    setExtClk(prescale);
  } else {
    // set a default frequency
    setPWMFreq(1000);
  }
  // set the default internal frequency
  setOscillatorFrequency(FREQUENCY_OSCILLATOR);
  this->inited=true;
}

inline bool PCA9685::isInited() {
  return this->inited;
}

inline void PCA9685::reset() {
  write8(PCA9685_MODE1, MODE1_RESTART);
  delay(10);
}

inline void PCA9685::sleep() {
  uint8_t awake = read8(PCA9685_MODE1);
  uint8_t sleep = awake | MODE1_SLEEP; // set sleep bit high
  write8(PCA9685_MODE1, sleep);
  delay(5); // wait until cycle ends for sleep to be active
}

inline void PCA9685::wake() {
  uint8_t sleep = read8(PCA9685_MODE1);
  uint8_t wakeup = sleep & ~MODE1_SLEEP; // set sleep bit low
  write8(PCA9685_MODE1, wakeup);
}

inline void PCA9685::setExtClk(uint8_t prescale) {
  uint8_t oldmode = read8(PCA9685_MODE1);
  uint8_t newmode = (oldmode & ~MODE1_RESTART) | MODE1_SLEEP; // sleep
  write8(PCA9685_MODE1, newmode); // go to sleep, turn off internal oscillator

  // This sets both the SLEEP and EXTCLK bits of the MODE1 register to switch to
  // use the external clock.
  write8(PCA9685_MODE1, (newmode |= MODE1_EXTCLK));

  write8(PCA9685_PRESCALE, prescale); // set the prescaler

  delay(5);
  // clear the SLEEP bit to start
  write8(PCA9685_MODE1, (newmode & ~MODE1_SLEEP) | MODE1_RESTART | MODE1_AI);

#ifdef PRINT_DEBUG_INFO
  Serial.print("Mode now 0x");
  Serial.println(read8(PCA9685_MODE1), HEX);
#endif
}

inline void PCA9685::setPWMFreq(float freq) {
#ifdef PRINT_DEBUG_INFO
  Serial.print("Attempting to set freq ");
  Serial.println(freq);
#endif
  // Range output modulation frequency is dependant on oscillator
  if (freq < 1)
    freq = 1;
  if (freq > 3500)
    freq = 3500; // Datasheet limit is 3052=50MHz/(4*4096)

  float prescaleval = ((_oscillator_freq / (freq * 4096.0)) + 0.5) - 1;
  if (prescaleval < PCA9685_PRESCALE_MIN)
    prescaleval = PCA9685_PRESCALE_MIN;
  if (prescaleval > PCA9685_PRESCALE_MAX)
    prescaleval = PCA9685_PRESCALE_MAX;
  uint8_t prescale = (uint8_t)prescaleval;

#ifdef PRINT_DEBUG_INFO
  Serial.print("Final pre-scale: ");
  Serial.println(prescale);
#endif

  uint8_t oldmode = read8(PCA9685_MODE1);
  uint8_t newmode = (oldmode & ~MODE1_RESTART) | MODE1_SLEEP; // sleep
  write8(PCA9685_MODE1, newmode);                             // go to sleep
  write8(PCA9685_PRESCALE, prescale); // set the prescaler
  write8(PCA9685_MODE1, oldmode);
  delay(5);
  // This sets the MODE1 register to turn on auto increment.
  write8(PCA9685_MODE1, oldmode | MODE1_RESTART | MODE1_AI);

#ifdef PRINT_DEBUG_INFO
  Serial.print("Mode now 0x");
  Serial.println(read8(PCA9685_MODE1), HEX);
#endif
}

inline void PCA9685::setOutputMode(bool totempole) {
  uint8_t oldmode = read8(PCA9685_MODE2);
  uint8_t newmode;
  if (totempole) {
    newmode = oldmode | MODE2_OUTDRV;
  } else {
    newmode = oldmode & ~MODE2_OUTDRV;
  }
  write8(PCA9685_MODE2, newmode);
#ifdef PRINT_DEBUG_INFO
  Serial.print("Setting output mode: ");
  Serial.print(totempole ? "totempole" : "open drain");
  Serial.print(" by setting MODE2 to ");
  Serial.println(newmode);
#endif
}

inline uint8_t PCA9685::readPrescale(void) {
  return read8(PCA9685_PRESCALE);
}

inline uint8_t PCA9685::getPWM(uint8_t num) {
  _i2c->requestFrom((int)_i2caddr, PCA9685_LED0_ON_L + 4 * num, (int)4);
  return _i2c->read();
}

inline void PCA9685::setPWM(uint8_t num, uint16_t on, uint16_t off) {
#ifdef PRINT_DEBUG_INFO
  Serial.print("Setting PWM ");
  Serial.print(num);
  Serial.print(": ");
  Serial.print(on);
  Serial.print("->");
  Serial.println(off);
#endif

  _i2c->beginTransmission(_i2caddr);
  _i2c->write(PCA9685_LED0_ON_L + 4 * num);
  _i2c->write(on);
  _i2c->write(on >> 8);
  _i2c->write(off);
  _i2c->write(off >> 8);
  _i2c->endTransmission();
}

inline void PCA9685::setPin(uint8_t num, uint16_t val, bool invert) {
  // Clamp value between 0 and 4095 inclusive.
  val = min(val, (uint16_t)4095);
  if (invert) {
    if (val == 0) {
      // Special value for signal fully on.
      setPWM(num, 4096, 0);
    } else if (val == 4095) {
      // Special value for signal fully off.
      setPWM(num, 0, 4096);
    } else {
      setPWM(num, 0, 4095 - val);
    }
  } else {
    if (val == 4095) {
      // Special value for signal fully on.
      setPWM(num, 4096, 0);
    } else if (val == 0) {
      // Special value for signal fully off.
      setPWM(num, 0, 4096);
    } else {
      setPWM(num, 0, val);
    }
  }
}

inline void PCA9685::writeMicroseconds(uint8_t num,
                                                uint16_t Microseconds) {
#ifdef PRINT_DEBUG_INFO
  Serial.print("Setting PWM Via Microseconds on output");
  Serial.print(num);
  Serial.print(": ");
  Serial.print(Microseconds);
  Serial.println("->");
#endif

  double pulse = Microseconds;
  double pulselength;
  pulselength = 1000000; // 1,000,000 us per second

  // Read prescale
  uint16_t prescale = readPrescale();

#ifdef PRINT_DEBUG_INFO
  Serial.print(prescale);
  Serial.println(" PCA9685 chip prescale");
#endif

  // Calculate the pulse for PWM based on Equation 1 from the datasheet section
  // 7.3.5
  prescale += 1;
  pulselength *= prescale;
  pulselength /= _oscillator_freq;

#ifdef PRINT_DEBUG_INFO
  Serial.print(pulselength);
  Serial.println(" us per bit");
#endif

  pulse /= pulselength;

#ifdef PRINT_DEBUG_INFO
  Serial.print(pulse);
  Serial.println(" pulse for PWM");
#endif

  setPWM(num, 0, pulse);
}

inline uint32_t PCA9685::getOscillatorFrequency(void) {
  return _oscillator_freq;
}

inline void PCA9685::setOscillatorFrequency(uint32_t freq) {
  _oscillator_freq = freq;
}

/******************* Low level I2C interface */
inline uint8_t PCA9685::read8(uint8_t addr) {
  _i2c->beginTransmission(_i2caddr);
  _i2c->write(addr);
  _i2c->endTransmission();

  _i2c->requestFrom((uint8_t)_i2caddr, (uint8_t)1);
  return _i2c->read();
}

inline void PCA9685::write8(uint8_t addr, uint8_t d) {
  _i2c->beginTransmission(_i2caddr);
  _i2c->write(addr);
  _i2c->write(d);
  _i2c->endTransmission();
}
#endif