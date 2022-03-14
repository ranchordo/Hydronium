#ifndef HYDRONIUM_UTIL
#define HYDRONIUM_UTIL

typedef uint64_t ConfigurationFunctionPtr;

#include "Arduino.h"
#include "uRTCLib.h"
#include "esp_wifi.h"
#include "WiFi.h"
#include "Interfaces.h"
#include "time.h"


#ifndef ENV_WIFI_SSID
#define ENV_WIFI_SSID "No SSID provided."
#endif

#ifndef ENV_WIFI_PWD
#define ENV_WIFI_PWD ""
#endif

class HydroniumUtil {
    public:
    static char *toCStr(String s);
    static bool setRTCWithNTP(InfoInterface* interface, uRTCLib *rtc);
};
//Used for when we have a PRValue but need to limit the address operator in scope
template<typename T> uint64_t functionPtr(T t) {
    return *reinterpret_cast<uint64_t*>(&t);
}

inline char *HydroniumUtil::toCStr(String s) {
    return (char *)s.c_str();
}

inline bool HydroniumUtil::setRTCWithNTP(InfoInterface* interface, uRTCLib *rtc) {
    esp_wifi_start();
    WiFi.mode(WIFI_STA);
    WiFi.begin(HydroniumUtil::toCStr(ENV_WIFI_SSID), HydroniumUtil::toCStr(ENV_WIFI_PWD));
    #ifdef PRINT_IMPORTANT_DEBUG_INFO
    interface->println("Connecting to " + String(ENV_WIFI_SSID));
    #endif
    delay(500);
    byte count = 0;
    while (WiFi.status() != WL_CONNECTED && count < 20)
    {
    #ifdef PRINT_IMPORTANT_DEBUG_INFO
        interface->print(".");
    #endif
        delay(500);
        count++;
    }
    if (count >= 20)
    {
    #ifdef PRINT_IMPORTANT_DEBUG_INFO
        interface->println("Failed to connect to wifi network.");
    #endif
        WiFi.disconnect(true);
        WiFi.mode(WIFI_MODE_NULL);
        esp_wifi_deinit();
        esp_wifi_stop();
        return false;
    }
    #ifdef PRINT_IMPORTANT_DEBUG_INFO
    interface->println("");
    #endif
    configTime(-25200, 0, "pool.ntp.org");
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
    #ifdef PRINT_IMPORTANT_DEBUG_INFO
        interface->println("Failed to get local time from ntp.");
    #endif
        WiFi.disconnect(true);
        WiFi.mode(WIFI_MODE_NULL);
        esp_wifi_stop();
        esp_wifi_deinit(); // So hardware entropy sources will work later
        return false;
    }
    #ifdef PRINT_IMPORTANT_DEBUG_INFO
    interface->print(String(timeinfo.tm_hour & 0xFF));
    interface->print(":");
    interface->print(String(timeinfo.tm_min & 0xFF));
    interface->print(":");
    interface->print(String(timeinfo.tm_sec & 0xFF));
    interface->print(", ");
    interface->print(String(timeinfo.tm_hour & 0xFF));
    interface->print("/");
    interface->print(String(timeinfo.tm_mon & 0xFF));
    interface->print("/");
    interface->print(String(timeinfo.tm_mday & 0xFF));
    interface->print("/");
    interface->println(String((timeinfo.tm_year % 100) & 0xFF));
    #endif
    rtc->set(
        timeinfo.tm_sec & 0xFF,
        timeinfo.tm_min & 0xFF,
        timeinfo.tm_hour & 0xFF,
        timeinfo.tm_wday & 0xFF,
        timeinfo.tm_mday & 0xFF,
        timeinfo.tm_mon & 0xFF,
        (timeinfo.tm_year % 100) & 0xFF); // Actually set the RTC
    WiFi.disconnect(true);
    WiFi.mode(WIFI_MODE_NULL);
    esp_wifi_stop();
    esp_wifi_deinit(); // So hardware entropy sources will work later
    return true;
}


#include "Regexp.h"

#define CONFIG_REGEXP "(%u*)%(%s*([^,]+)%s*,%s*([^,]+)%s*%)"
#define MAX_INFO_LEN 32
#define MAXCAPTURES 4

#define MAX_CONFIG_PARAMETERS 64

struct ReflectedStructureField {
    String type;
    String name;
    bool config;
    uint32_t offset;
};

class ReflectableStructExtractor {
    public:
    static void structExtractionRegexMatchCallback(const char* match, const unsigned int length, const MatchState& ms);
    static char regexpCaptureBuffer[MAX_INFO_LEN];

    ReflectableStructExtractor() : ms() {}
    void extractStructInformation(const String reflectionData);
    ReflectedStructureField* getResults() {return results;}
    uint8_t getNumResults() {return numResults;}
    private:
    static ReflectableStructExtractor* activeExtractor;
    MatchState ms;
    ReflectedStructureField results[MAX_CONFIG_PARAMETERS];
    uint8_t numResults=0;
    uint32_t offset=0;
    static uint32_t sizeofType(String name, InfoInterface* interface);
};
char ReflectableStructExtractor::regexpCaptureBuffer[MAX_INFO_LEN];
ReflectableStructExtractor* ReflectableStructExtractor::activeExtractor=nullptr;

inline void ReflectableStructExtractor::extractStructInformation(const String reflectionData) {
    numResults=0;
    offset=0;
    activeExtractor=this;
    ms.Target(HydroniumUtil::toCStr("; "+reflectionData));
    ms.GlobalMatch(CONFIG_REGEXP, &ReflectableStructExtractor::structExtractionRegexMatchCallback);
}
inline void ReflectableStructExtractor::structExtractionRegexMatchCallback(const char * match, const unsigned int length, const MatchState& ms) {
    ms.GetCapture(regexpCaptureBuffer,0);
    activeExtractor->results[activeExtractor->numResults].config=String(regexpCaptureBuffer).equals("CONFIG");

    ms.GetCapture(regexpCaptureBuffer,1);
    activeExtractor->results[activeExtractor->numResults].type=String(regexpCaptureBuffer);

    activeExtractor->results[activeExtractor->numResults].offset=activeExtractor->offset;
    activeExtractor->offset+=sizeofType(String(regexpCaptureBuffer),nullptr);

    ms.GetCapture(regexpCaptureBuffer,2);
    activeExtractor->results[activeExtractor->numResults].name=String(regexpCaptureBuffer);
    activeExtractor->numResults++;
}

#define ADD_TYPE_SIZE(type) if( name.equals( #type )) {return sizeof( type );}
inline uint32_t ReflectableStructExtractor::sizeofType(String name, InfoInterface* interface) {
    ADD_TYPE_SIZE(int);
    ADD_TYPE_SIZE(float);
    ADD_TYPE_SIZE(long);
    ADD_TYPE_SIZE(double);
    ADD_TYPE_SIZE(ConfigurationFunctionPtr);
    if(interface!=nullptr) {
        interface->log(3,"Type "+name+" not recognized.");
    } else {
        Serial.println("Error: Type "+name+" is not recognized.");
    }
    return 0;
}
#undef ADD_TYPE_SIZE

#endif