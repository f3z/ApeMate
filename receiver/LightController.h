#ifndef _LIGHTCONTROLLER_H_
#define _LIGHTCONTROLLER_H_

#include "Device.h"
#include "Led.h"

/// Arduino pin number the LEDs are connected to
constexpr uint8_t RED_LED_PIN = 5_u8;   // 6
constexpr uint8_t GREEN_LED_PIN = 6_u8; // 5
constexpr uint8_t BLUE_LED_PIN = 9_u8;  // 9
using RED_PWM_DRIVER = Pwm_Arduino<RED_LED_PIN>;
using GREEN_PWM_DRIVER = Pwm_Arduino<GREEN_LED_PIN>;
using BLUE_PWM_DRIVER = Pwm_Arduino<BLUE_LED_PIN>;

// using RED_PWM_DRIVER = Pwm_Pin6;
// using GREEN_PWM_DRIVER = Pwm_Pin5;
// using BLUE_PWM_DRIVER = Pwm_Pin9;

/**
 * Class to manage the overall control of RGB LED strip.
 */
class LightController
{
public:
    LightController();

    /// Initiailise registers if required
    void Setup();

    /// Main loop function
    void DoIteration();

    /**
     * Set colour between red and green.
     * range at 0 = green, 255 = red;
     */
    void SetColour(uint8_t range);

private:
    Led<RED_PWM_DRIVER, 1_u16, 500_u16> redLed;
    Led<GREEN_PWM_DRIVER, 1_u16, 500_u16> greenLed;
    Led<BLUE_PWM_DRIVER, 1_u16, 500_u16> blueLed;

    uint16_t rgbValues[3];
    /// Value of either 0, 1, 2. Points to index of the colour being modified
    uint8_t majorColourIndex;

    // Target rgb
    uint16_t targetRgbValues[3];

    /**
     * Adjust the value of rgbValues elements per call according to majorColourIndex
     * Update majorColourIndex when the colour have cycled through 0-255
     * Cycle Algorithm: 
     *      Brightness = -127.5*cos(x/255*PI) + 127.5 , from x = [0, 255], y = [0, 255]
     *      Two LEDs will have brightness 180deg out of phase so they complement
     */
    void CycleColours();
};


#endif
