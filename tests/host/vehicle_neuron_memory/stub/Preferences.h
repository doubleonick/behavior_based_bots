#pragma once
#include <stdint.h>
struct Preferences { bool begin(const char*, bool){return true;} void end(){} void putUChar(const char*,uint8_t){} void putFloat(const char*,float){} void putLong(const char*,long){}
 uint8_t getUChar(const char*,uint8_t d){return d;} float getFloat(const char*,float d){return d;} long getLong(const char*,long d){return d;} bool isKey(const char*){return false;} };
