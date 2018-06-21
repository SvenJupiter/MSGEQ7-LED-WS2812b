#include <Arduino.h>
#include <LEDString.h>
#include <Tools.h>

#define DEBUG true

#include <FastLED.h>
#define LED_PIN 21
#define NUMBER_OF_LEDS 300
// CRGBArray<NUMBER_OF_LEDS> leds;
CRGB leds[NUMBER_OF_LEDS]; // Kein Template Parameter :-)
SoundLight::LEDString complete_string(leds, NUMBER_OF_LEDS, false);


// MSGEQ7
/*
    Reads MSGEQ7 IC with 7 different frequencys from range 0-255
    63Hz, 160Hz, 400Hz, 1kHz, 2.5kHz, 6.25KHz, 16kHz
*/
#include <Equalizer.h>
// #include "MSGEQ7.h"
#define pinAnalogLeft A0
#define pinAnalogRight A1
#define pinReset 5
#define pinStrobe 4
#define MSGEQ7_INTERVAL ReadsPerSecond(60)
#define MSGEQ7_SMOOTH 64

// // CMSGEQ7-Objekt anlegen
// CMSGEQ7<MSGEQ7_SMOOTH, pinReset, pinStrobe, pinAnalogLeft, pinAnalogRight> MSGEQ7;
// Equalizer-Objekt anlegen
SoundLight::Equalizer<MSGEQ7_SMOOTH, pinReset, pinStrobe, pinAnalogLeft, pinAnalogRight> MSGEQ7;


// Ein paar Konstanten
#define LEFT_CHANNEL 0
#define RIGHT_CHANNEL 1
#define NUMBER_OF_FREQUENCIES 7
#define NUMBER_OF_COLORS 3

// Ein Array um die eingelesenen Werte abzuspeichern
uint8_t raw_frequency[NUMBER_OF_FREQUENCIES] = {0, 0, 0, 0, 0, 0, 0};
uint8_t color_frequency[NUMBER_OF_COLORS] = {0, 0, 0};


// Sieben Streifen, eine ander Farbe für jede Frequenz
SoundLight::LEDString color_strings[10] = {
    complete_string.slice(0, 30),
    complete_string.slice(30, 60).reverse(),
    complete_string.slice(60, 90),
    complete_string.slice(90, 120).reverse(),
    complete_string.slice(120, 150),
    complete_string.slice(150, 180).reverse(),
    complete_string.slice(180, 210),
    complete_string.slice(210, 240).reverse(),
    complete_string.slice(240, 270),
    complete_string.slice(270, 300).reverse(),
};

namespace HSV_Color 
{
    CHSV Red = CHSV(0, 255, 255);
    CHSV Orange = CHSV(32, 255, 255);
    CHSV Yellow = CHSV(64, 255, 255);
    CHSV Lime = CHSV(96, 255, 255);
    CHSV Aqua = CHSV(128, 255, 255);
    CHSV Blue = CHSV(160, 255, 255);
    CHSV Purple = CHSV(192, 255, 255);
    CHSV White = CHSV(132, 0, 255);
}

void setup() {

        // Setup FastLED
    FastLED.setBrightness(64);
    FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUMBER_OF_LEDS);
    for (uint32_t i = 0; i < NUMBER_OF_LEDS; ++i)
        complete_string.colorize(i, CRGB(0,0,0));// leds[i] = CRGB(0, 4, 0);
    FastLED.show();


    // This will set the IC ready for reading
    MSGEQ7.begin();

    if (DEBUG)
    {   
        // Serial setup with faster baud rate gives more FPS (115200)
        Serial.begin(115200);
        Serial.println(F("Startup"));
    }


    MSGEQ7.print_equalizer_setting();

    SoundLight::Tools::blink(13, 2, 2000);





    // Testen ob Zugrifsrichtung stimmt
    // color_strings[6].self_test(HSV_Color::Red);
    // color_strings[5].self_test(HSV_Color::Orange);
    // color_strings[4].self_test(HSV_Color::Yellow);
    // color_strings[3].self_test(HSV_Color::Lime);
    // color_strings[2].self_test(HSV_Color::Aqua);
    // color_strings[1].self_test(HSV_Color::Blue);
    // color_strings[0].self_test(HSV_Color::Purple);
    // color_strings[7].self_test(HSV_Color::White);
    // color_strings[8].self_test(HSV_Color::White);
    // color_strings[9].self_test(HSV_Color::White);

    color_strings[7].colorize(CRGB(32,32,32));
    color_strings[8].colorize(CRGB(32,32,32));
    color_strings[9].colorize(CRGB(32,32,32));


    FastLED.setBrightness(8);
    FastLED.show();
}

void loop() {

//   // Analyze MSGEQ7 ICs now. Will always return true.
//   bool newReading = MSGEQ7.read();

  // Analyze without delay every interval
  bool newReading = MSGEQ7.read(MSGEQ7_INTERVAL);

    if (newReading)
    {
        // Ferquenz-Werte einlesen
        for (uint8_t i = 0; i < NUMBER_OF_FREQUENCIES; ++i)
        {
            raw_frequency[i] = MSGEQ7[i];
        }

        // if (DEBUG)
        // {
        //     // Ausgeben
        //     for (uint8_t i = 0; i < NUMBER_OF_FREQUENCIES; ++i)
        //     {
        //         Serial.print(raw_frequency[i]); Serial.print('\t');
        //     }
        //     Serial.println(MSGEQ7.volume());
        // }


        // HSV_Color::Purple.value = raw_frequency[6];
        // HSV_Color::Blue.value = raw_frequency[5];
        // HSV_Color::Aqua.value = raw_frequency[4];
        // HSV_Color::Lime.value = raw_frequency[3];
        // HSV_Color::Yellow.value = raw_frequency[2];
        // HSV_Color::Orange.value = raw_frequency[1];
        // HSV_Color::Red.value = raw_frequency[0];
        // HSV_Color::White.value = raw_frequency[MSGEQ7.volume()];

        color_strings[0].colorize(HSV_Color::Purple,    SoundLight::Tools::mapInput(raw_frequency[6], 0, 255, 1, 30));
        color_strings[1].colorize(HSV_Color::Blue,      SoundLight::Tools::mapInput(raw_frequency[5], 0, 255, 1, 30));
        color_strings[2].colorize(HSV_Color::Aqua,      SoundLight::Tools::mapInput(raw_frequency[4], 0, 255, 1, 30));
        color_strings[3].colorize(HSV_Color::Lime,      SoundLight::Tools::mapInput(raw_frequency[3], 0, 255, 1, 30));
        color_strings[4].colorize(HSV_Color::Yellow,    SoundLight::Tools::mapInput(raw_frequency[2], 0, 255, 1, 30));
        color_strings[5].colorize(HSV_Color::Orange,    SoundLight::Tools::mapInput(raw_frequency[1], 0, 255, 1, 30));
        color_strings[6].colorize(HSV_Color::Red,       SoundLight::Tools::mapInput(raw_frequency[0], 0, 255, 1, 30));
        color_strings[9].colorize(HSV_Color::White,     SoundLight::Tools::mapInput(mapNoise(MSGEQ7.getVolume()),  0, 255, 1, 30));
        color_strings[8].colorize(HSV_Color::White,     SoundLight::Tools::mapInput(MSGEQ7.get_normalized_volume(),  0, 255, 1, 30));
        color_strings[7].colorize(HSV_Color::White,     SoundLight::Tools::mapInput(MSGEQ7.get_equalized_volume(),  0, 255, 1, 30));
        
    }

    FastLED.show();
}
