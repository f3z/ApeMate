#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <Arduino.h>

// Global debug toggle
// #define DEBUG true

#ifndef DEBUG
#define DEBUG false
#endif

/**
 * Custom defined types and literals for portability
 */
// using uint8_t = byte;
// using int8_t = char;
// using uint16_t = unsigned int;
// using int16_t = int;
// using uint32_t = unsigned long;
// using int32_t = long;

constexpr uint8_t operator "" _u8(unsigned long long int val)
{
    return byte(val);
    // return static_cast<uint8_t>(val);
}

constexpr uint16_t operator "" _u16(unsigned long long int val)
{
    return static_cast<uint16_t>(val);
}

// #ifdef DEBUG
//     #define DEBUG_LOG(val) (Serial.write(val))
// #else
//     #undef DEBUG_LOG(val)
// #endif

auto DEBUG_LOG = [](String str)
{
    if (DEBUG)
    {
        Serial.println(str);
    }

    // else do nothing.
};

#endif