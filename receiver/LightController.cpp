#include "LightController.h"
#include "Device.h"

// Local namespace
namespace {

template <uint8_t MAX, typename T>
constexpr T ModuloIncrement(const T inputVal)
{
    return (inputVal >= MAX - 1_u8) ? static_cast<T>(0_u8) : static_cast<T>(inputVal + 1_u8);
}

template <uint8_t MAX, typename T>
constexpr T ModuloDecrement(const T inputVal)
{
    return (inputVal == 0) ? static_cast<T>(MAX - 1_u8) : static_cast<T>(inputVal - 1_u8);
}

/// Look up table generated from y = -127.5*cos(x/255*PI) + 127.5 , for x = [0, 255], y = [0, 255]
constexpr uint8_t OUTPUT_SHAPER[256] = {
    1_u8, 1_u8, 1_u8, 1_u8, 1_u8, 1_u8, 1_u8, 1_u8, 1_u8, 1_u8, 
    // 0_u8, 0_u8, 0_u8, 0_u8, 0_u8, 0_u8, 0_u8, 0_u8, 1_u8, 1_u8, 
    1_u8, 1_u8, 1_u8, 2_u8, 2_u8, 2_u8, 2_u8, 3_u8, 3_u8, 3_u8, 
    4_u8, 4_u8, 5_u8, 5_u8, 6_u8, 6_u8, 6_u8, 7_u8, 8_u8, 8_u8, 
    9_u8, 9_u8, 10_u8, 10_u8, 11_u8, 12_u8, 12_u8, 13_u8, 14_u8, 14_u8, 
    15_u8, 16_u8, 17_u8, 17_u8, 18_u8, 19_u8, 20_u8, 21_u8, 22_u8, 23_u8, 
    23_u8, 24_u8, 25_u8, 26_u8, 27_u8, 28_u8, 29_u8, 30_u8, 31_u8, 32_u8, 
    33_u8, 34_u8, 35_u8, 37_u8, 38_u8, 39_u8, 40_u8, 41_u8, 42_u8, 43_u8, 
    45_u8, 46_u8, 47_u8, 48_u8, 49_u8, 51_u8, 52_u8, 53_u8, 54_u8, 56_u8, 
    57_u8, 58_u8, 60_u8, 61_u8, 62_u8, 64_u8, 65_u8, 66_u8, 68_u8, 69_u8, 
    71_u8, 72_u8, 73_u8, 75_u8, 76_u8, 78_u8, 79_u8, 81_u8, 82_u8, 84_u8, 
    85_u8, 87_u8, 88_u8, 90_u8, 91_u8, 93_u8, 94_u8, 96_u8, 97_u8, 99_u8, 
    100_u8, 102_u8, 103_u8, 105_u8, 106_u8, 108_u8, 109_u8, 111_u8, 113_u8, 114_u8, 
    116_u8, 117_u8, 119_u8, 120_u8, 122_u8, 124_u8, 125_u8, 127_u8, 128_u8, 130_u8, 
    131_u8, 133_u8, 135_u8, 136_u8, 138_u8, 139_u8, 141_u8, 142_u8, 144_u8, 146_u8, 
    147_u8, 149_u8, 150_u8, 152_u8, 153_u8, 155_u8, 156_u8, 158_u8, 159_u8, 161_u8, 
    162_u8, 164_u8, 165_u8, 167_u8, 168_u8, 170_u8, 171_u8, 173_u8, 174_u8, 176_u8, 
    177_u8, 179_u8, 180_u8, 182_u8, 183_u8, 184_u8, 186_u8, 187_u8, 189_u8, 190_u8, 
    191_u8, 193_u8, 194_u8, 195_u8, 197_u8, 198_u8, 199_u8, 201_u8, 202_u8, 203_u8, 
    204_u8, 206_u8, 207_u8, 208_u8, 209_u8, 210_u8, 212_u8, 213_u8, 214_u8, 215_u8, 
    216_u8, 217_u8, 218_u8, 220_u8, 221_u8, 222_u8, 223_u8, 224_u8, 225_u8, 226_u8, 
    227_u8, 228_u8, 229_u8, 230_u8, 231_u8, 232_u8, 232_u8, 233_u8, 234_u8, 235_u8, 
    236_u8, 237_u8, 238_u8, 238_u8, 239_u8, 240_u8, 241_u8, 241_u8, 242_u8, 243_u8, 
    243_u8, 244_u8, 245_u8, 245_u8, 246_u8, 246_u8, 247_u8, 247_u8, 248_u8, 249_u8, 
    249_u8, 249_u8, 250_u8, 250_u8, 251_u8, 251_u8, 252_u8, 252_u8, 252_u8, 253_u8, 
    253_u8, 253_u8, 253_u8, 254_u8, 254_u8, 254_u8, 254_u8, 254_u8, 255_u8, 255_u8, 
    255_u8, 255_u8, 255_u8, 255_u8, 255_u8, 255_u8
};

// /// Look up table to further shape the red LED output
// // NB: Red to blue/green power should be approx 60 : 95
// // So red LED have it's own scaling according to
// // Brightness = e^((x+60)/60) - 1 , from x = [10, 255], y = [0, 191]
// // for x = [0, 9], output been modified to output 0 and 1 instead of starting from 2.
// // This tracks the 1/30 scaling to x=100, then shoots up to increase red brightness
// constexpr uint8_t RED_OUTPUT_PRESCALE[256] = {
//     // Modified first 10 elements to get 0-1
//     0_u8, 1_u8, 2_u8, 3_u8, 4_u8, 5_u8, 5_u8, 5_u8, 5_u8, 5_u8, 
//     5_u8, 5_u8, 5_u8, 5_u8, 5_u8, 5_u8, 5_u8, 5_u8, 5_u8, 5_u8, 
//     5_u8, 5_u8, 5_u8, 5_u8, 5_u8, 6_u8, 6_u8, 6_u8, 6_u8, 6_u8, 
//     6_u8, 6_u8, 6_u8, 6_u8, 6_u8, 6_u8, 6_u8, 6_u8, 6_u8, 6_u8, 
//     6_u8, 6_u8, 7_u8, 7_u8, 7_u8, 7_u8, 7_u8, 7_u8, 7_u8, 7_u8, 
//     7_u8, 7_u8, 7_u8, 7_u8, 7_u8, 7_u8, 7_u8, 8_u8, 8_u8, 8_u8, 
//     8_u8, 8_u8, 8_u8, 8_u8, 8_u8, 8_u8, 8_u8, 8_u8, 8_u8, 8_u8, 
//     8_u8, 9_u8, 9_u8, 9_u8, 9_u8, 9_u8, 9_u8, 9_u8, 9_u8, 9_u8, 
//     9_u8, 9_u8, 9_u8, 10_u8, 10_u8, 10_u8, 10_u8, 10_u8, 10_u8, 10_u8, 
//     10_u8, 10_u8, 10_u8, 10_u8, 11_u8, 11_u8, 11_u8, 11_u8, 11_u8, 11_u8, 
//     11_u8, 11_u8, 11_u8, 11_u8, 12_u8, 12_u8, 12_u8, 12_u8, 12_u8, 12_u8, 
//     12_u8, 12_u8, 12_u8, 13_u8, 13_u8, 13_u8, 13_u8, 13_u8, 13_u8, 13_u8, 
//     13_u8, 14_u8, 14_u8, 14_u8, 14_u8, 14_u8, 14_u8, 14_u8, 14_u8, 15_u8, 
//     15_u8, 15_u8, 15_u8, 15_u8, 15_u8, 15_u8, 15_u8, 16_u8, 16_u8, 16_u8, 
//     16_u8, 16_u8, 16_u8, 16_u8, 17_u8, 17_u8, 17_u8, 17_u8, 17_u8, 17_u8, 
//     17_u8, 18_u8, 18_u8, 18_u8, 18_u8, 18_u8, 18_u8, 19_u8, 19_u8, 19_u8, 
//     19_u8, 19_u8, 19_u8, 20_u8, 20_u8, 20_u8, 20_u8, 20_u8, 20_u8, 21_u8, 
//     21_u8, 21_u8, 21_u8, 21_u8, 22_u8, 22_u8, 22_u8, 22_u8, 22_u8, 23_u8, 
//     23_u8, 23_u8, 23_u8, 23_u8, 24_u8, 24_u8, 24_u8, 24_u8, 24_u8, 25_u8, 
//     25_u8, 25_u8, 25_u8, 25_u8, 26_u8, 26_u8, 26_u8, 26_u8, 27_u8, 27_u8, 
//     27_u8, 27_u8, 28_u8, 28_u8, 28_u8, 28_u8, 28_u8, 29_u8, 29_u8, 29_u8, 
//     29_u8, 30_u8, 30_u8, 30_u8, 31_u8, 31_u8, 31_u8, 31_u8, 32_u8, 32_u8, 
//     32_u8, 32_u8, 33_u8, 33_u8, 33_u8, 34_u8, 34_u8, 34_u8, 34_u8, 35_u8, 
//     35_u8, 35_u8, 36_u8, 36_u8, 36_u8, 37_u8, 37_u8, 37_u8, 37_u8, 38_u8, 
//     38_u8, 38_u8, 39_u8, 39_u8, 39_u8, 40_u8, 40_u8, 40_u8, 41_u8, 41_u8, 
//     42_u8, 42_u8, 42_u8, 43_u8, 43_u8, 43_u8
// };

} /* namespace */

LightController::LightController()
    : redLed(),
      greenLed(),
      blueLed(),
      /// Start with blue
      rgbValues({0_u16, 0_u16, 0_u16}),
      /// Must match rgbValues's major element
      majorColourIndex(2_u8),
      targetRgbValues({0_u16, 0_u16, 0_u16})
{
}

void LightController::Setup()
{
    redLed.Setup();
    greenLed.Setup();
    blueLed.Setup();
}

void LightController::DoIteration()
{
    CycleColours();

    // Set the final output
    // redLed.SetBrightness(OUTPUT_SHAPER[rgbValues[0]]);
    // greenLed.SetBrightness(OUTPUT_SHAPER[rgbValues[1]]);
    // blueLed.SetBrightness(OUTPUT_SHAPER[rgbValues[2]]);

    redLed.SetBrightness(rgbValues[0]);
    greenLed.SetBrightness(rgbValues[1]);
    blueLed.SetBrightness(0);

    DEBUG_LOG(String("rgbValues=" + String(rgbValues[0]) + "," + String(rgbValues[1]) + "," + String(rgbValues[2])));
}


void LightController::CycleColours()
{
//    // Check if the major colour needs to be updated
//    if (rgbValues[majorColourIndex] == 0_u8)
//    {
//        majorColourIndex = ModuloIncrement<3>(majorColourIndex);
//    }

//    const uint8_t nextIndex = ModuloIncrement<3>(majorColourIndex);

    for (size_t i = 0; i < 3; ++i)
    {
      if (targetRgbValues[i] > rgbValues[i])
      {
          rgbValues[i] = ModuloIncrement<256>(rgbValues[i]);
      }
      else if (targetRgbValues[i] < rgbValues[i])
      {
          rgbValues[i] = ModuloDecrement<256>(rgbValues[i]);
      }
    }
}

void LightController::SetColour(uint8_t range)
{
    // Red
    targetRgbValues[0] = range;
    
    // Green
    targetRgbValues[1] = 255 - range;

    // Blue = 0
    rgbValues[2] = 0;
}

