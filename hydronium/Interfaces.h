#ifndef OUTPUTS
#define OUTPUTS

#define MAX_INTERFACES_IN_COLLECTION 0x10
#define PRINTLN(x) println(String(x))
#define PRINT(x) print(String(x))

#include "Arduino.h"
#include "uRTCLib.h"

const String prefixes[5] = { "[DEBUG]	", "[INFO]	", "[WARN]	", "[ERROR]	", "[FATAL]	" };

class InfoInterface {
public:
    InfoInterface() {}
    InfoInterface(uRTCLib* rtc) { this->rtc = rtc; this->RTCEnabled = true; }
    virtual void print(String s) {}
    virtual void println(String s) {}
    virtual void write(char* s) {}
    virtual unsigned int available() { return 0; }
    virtual void readBytes(char* buffer, unsigned int length) {}
    virtual String readString() { return ""; }
    void log(uint8_t level, String msg) {
        String timestamp = "	UNKNOWN	UNKNOWN	";
        if (this->RTCEnabled) {
            rtc->refresh();
            timestamp = "	" + String(rtc->month()) + "-" + String(rtc->day()) + "-" + String(rtc->year()) + "	" + String(rtc->hour()) + ":" + String(rtc->minute()) + ":" + String(rtc->second()) + "	";
        }
        this->println(timestamp + prefixes[level] + msg);
    }
    bool isRTCEnabled() { return RTCEnabled; }
    bool isBegun() { return begun; }
    void begin() {
        if (!begun) { begin_back(); begun = true; }
    }
protected:
    virtual void begin_back() {}
private:
    bool begun = false;
    uRTCLib* rtc;
    bool RTCEnabled = false;
};

class OutputInterfaceCollection: public InfoInterface {
public:
    OutputInterfaceCollection() {}
    OutputInterfaceCollection(uRTCLib* rtc): InfoInterface(rtc) {}
    void begin_back() override { for (InfoInterface* interface: this->interfaces) { interface->begin(); } }
    void print(String s) override { for (InfoInterface* interface: this->interfaces) { interface->print(s); } }
    void println(String s) override { for (InfoInterface* interface: this->interfaces) { interface->println(s); } }
    void write(char* s) override { for (InfoInterface* interface: this->interfaces) { interface->write(s); } }
    void addInterface(InfoInterface* interface) { interfaces[numInterfaces] = interface; numInterfaces++; }
    unsigned int getNumInterfaces() { return this->numInterfaces; }
private:
    InfoInterface* interfaces[MAX_INTERFACES_IN_COLLECTION];
    unsigned int numInterfaces = 0;
};

class SerialInterface: public InfoInterface {
public:
    SerialInterface(HardwareSerial* serial, unsigned long baud) { this->serial = serial; this->baud = baud; }
    SerialInterface(HardwareSerial* serial, unsigned long baud, uRTCLib* rtc): InfoInterface(rtc) { this->serial = serial; this->baud = baud; }
    void begin_back() override { this->serial->begin(baud); this->println("Serial interface OK at " + String(this->baud) + " baud"); }
    void print(String s) override { this->serial->print(s); }
    void println(String s) override { this->serial->println(s); }
    void write(char* s) override { this->serial->write(s); }
    unsigned int available() override { return this->serial->available(); }
    void readBytes(char* buffer, unsigned int length) override { this->serial->readBytes(buffer, length); }
    String readString() override { return this->serial->readString(); }
private:
    HardwareSerial* serial;
    unsigned long baud = 9600;
};

#endif