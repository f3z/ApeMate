#ifndef _LED_H_
#define _LED_H_

#include "Device.h"

/**
* Class to abstract a connected LED light.
*/
template <class PWM_DRIVER, uint16_t MIN_OUTPUT, uint16_t MAX_OUTPUT>
class Led
{
public:
    void Setup()
    {
        PWM_DRIVER::Setup();
    }

    /// Clean up by turning the LED off.
    ~Led()
    {
        // Turn off the LED
        SetBrightness(0_u16);
    }

    /// Set the LED brightness at value
    void SetBrightness(uint16_t value)
    {
        // Scale the input range to arduino's output range
        // Taking minimum output into account.
        // output = (maxOut - minOut)/255 * value + minOut

        uint16_t pwmOutput;

        if (value == 0_u16)
        {
            pwmOutput = 0_u16;
        }
        else
        {
            // Use 32 bit to avoid overflowing on 10bit multiplication
            constexpr uint32_t outputRange = MAX_OUTPUT - MIN_OUTPUT;
            pwmOutput = static_cast<uint16_t>(outputRange * value / PWM_DRIVER::MAX_OUTPUT) + MIN_OUTPUT;
        }

        // Send output to arduino pin
        // analogWrite(OUTPUT_PIN, pwmOutput);
        PWM_DRIVER::SetPwm(pwmOutput);
    }

};


// /// Specialised driver for pin6, Timer4D
// class Pwm_Pin6
// {
// public:
//     static void Setup()
//     {
//         // Default arduino initilisation. Probably forgot to enable some clock signal or GPIO outside of Timer4
//         analogWrite(6, 1);
//         // See Atmel page 164 Timer 4 control registers
//         TCCR4A = 0x2_u8;   // 0x2 -> PWM based on OCR4A, OC4A and OC4Abar disconnected same for OC4B and bar
//         TCCR4B = 0x7_u8;   // 0x7 -> 0 dead time, source prescaler CLK/64
//         TCCR4C = 0x9_u8;   // 0x9 -> PWM based on OCR4D, OC4D connected, clear on compare match when counting up, set on compare match when counting down
//         TCCR4D = 0x1_u8;   // 0x1 -> fault protection disabled, PWM4A and D set to phase and frequency correct PWM (TOP = OCR4C)
//         TCCR4E = 0x0_u8;   // 0x0 -> special features disabled
        
//         TIMSK4 = 0x0_u8;   // 0x0 -> all interrupts disabled

//         // Output compare register OCR4C (10bit), setting the PWM's TOP value = 0x3FF
//         // High byte register TC4H - shared
//             // to write 10 bits : write to TC4H, then write to low byte
//             // to read read from low byte, then from TC4H
//         TC4H = 0x3_u8;
//         OCR4C = 0xFF_u8;

//         // Turn off by default.
//         SetPwm(0_u16);
//     }

//     static void SetPwm(uint16_t value)
//     {
//         // Output compare register OCR4D (10bit), setting the PWM's duty cycle
//         // High byte register TC4H - shared
//             // to write 10 bits : write to TC4H, then write to low byte
//             // to read read from low byte, then from TC4H
//         constexpr uint8_t HIGH_10BIT_MASK = 0x3_u8;
//         constexpr uint8_t LOW_8BIT_MASK = 0xFF_u8;

//         TC4H = static_cast<uint8_t>(HIGH_10BIT_MASK & (static_cast<unsigned int>(value) >> 8_u8));
//         OCR4D = static_cast<uint8_t>(LOW_8BIT_MASK & value);

//         // DEBUG_LOG("OCR4DL = 0x" + String(OCR4D, HEX));
//         // DEBUG_LOG("TC4H = 0x" + String(TC4H, HEX));
//     }

//     /// Maximum 10 bit PWM output value
//     static constexpr uint16_t MAX_OUTPUT = 0x400_u16;
// };

// /// Specialised driver for pin9, Timer1A
// class Pwm_Pin9
// {
// public:
//     static void Setup()
//     {
//         // Default arduino initilisation. Probably forgot to enable some clock signal or GPIO outside of Timer1
//         analogWrite(9, 1);
//         // See Atmel page 131 Timer 1 control registers
//         TCCR1A = 0x83_u8;   // 0x83 -> clear OC1A on match, WGM11 = 1, WGM10 = 1 -> PWM phase correct, 10 bit, Update OCR1A at TOP (0xFF). (default arduino is 0x81 for 8 bit)
//         TCCR1B = 0x3_u8;    // 0x3 -> input capture disable, WGM13, 12 = 0, clock select = CLK/64
//         TCCR1C = 0x0_u8;    // 0x0 -> force compare match off
//         TIMSK1 = 0x0_u8;    // 0x0 -> all disabled

//         // Turn off by default.
//         SetPwm(0);
//     }

//     static void SetPwm(uint16_t value)
//     {
//         // 16 bit register
//         OCR1A = static_cast<uint16_t>(value & MAX_OUTPUT);
//         DEBUG_LOG("OCR1A = 0x" + String(OCR1A, HEX));
//     }

//     /// Maximum PWM output value limited to 10 bits
//     static constexpr uint16_t MAX_OUTPUT = 0x3FF_u16;
// };

// /// Specialised driver for pin5, Timer3A
// class Pwm_Pin5
// {
// public:
//     static void Setup()
//     {
//         // Default arduino initilisation. Probably forgot to enable some clock signal or GPIO outside of Timer3
//         analogWrite(5, 1);
//         // See Atmel page 131 Timer 3 control registers
//         TCCR3A = 0x83_u8;   // 0x83 -> clear OC1A on match, WGM11 = 1, WGM10 = 1 -> PWM phase correct, 10 bit, Update OCR1A at TOP (0xFF). (default arduino is 0x81 for 8 bit)
//         TCCR3B = 0x3_u8;    // 0x3 -> input capture disable, WGM13, 12 = 0, clock select = CLK/64
//         TCCR3C = 0x0_u8;    // 0x0 -> force compare match off
//         TIMSK3 = 0x0_u8;    // 0x0 -> all disabled

//         // Turn off by default.
//         SetPwm(0);
//     }

//     static void SetPwm(uint16_t value)
//     {
//         // 16 bit register
//         OCR3A = static_cast<uint16_t>(value & MAX_OUTPUT);
//         // DEBUG_LOG("OCR3A = 0x" + String(OCR3A, HEX));
//     }

//     /// Maximum PWM output value limited to 10 bits
//     static constexpr uint16_t MAX_OUTPUT = 0x3FF_u16;
// };

/// Default Arduino Core driver
template<uint8_t OUTPUT_PIN>
class Pwm_Arduino
{
public:
    static void Setup()
    {
    }

    static void SetPwm(uint16_t value)
    {
        // Arduino only supports 8 bit values.
        constexpr uint8_t UINT8T_MAX = 0xFF_u8;
        uint8_t pwmOut = (value > UINT8T_MAX) ? UINT8T_MAX : static_cast<uint8_t>(value);
        analogWrite(OUTPUT_PIN, pwmOut);
    }

    /// Maximum PWM output value
    static constexpr uint8_t MAX_OUTPUT = 0xFF_u8;
};

#endif