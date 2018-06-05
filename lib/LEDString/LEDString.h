#ifndef LEDSTRING_H
#define LEDSTRING_H


#include <Arduino.h>
#include <FastLED.h>




// #define number_of_leds 300

namespace SoundLight
{

class LEDBaseString
{
    public:

        LEDBaseString(CRGB* const leds, const uint32_t total_number_of_leds);
        LEDBaseString(const LEDBaseString& other);
        void operator=(const LEDBaseString& other);

        // virtual ~LEDBaseString(){}
        
        CRGB& led(const uint32_t index);
        const CRGB& led(const uint32_t index) const;

        CRGB& operator[](const uint32_t index) {return this->led(index);}
        const CRGB& operator[](const uint32_t index) const {return this->led(index);}

        // Getter
        CRGB* led_array() const {return this->leds;}
        uint32_t length() const {return this->total_number_of_leds;}

    private:

        CRGB* leds;
        uint32_t total_number_of_leds;

};

class LEDString : public LEDBaseString
{

    public:

        LEDString(CRGB* const leds, const uint32_t total_number_of_leds, uint32_t const offset, const uint32_t length);
        LEDString(CRGB* const leds, const uint32_t total_number_of_leds);
        LEDString(const LEDBaseString& leds, uint32_t const offset, const uint32_t length);
        LEDString(const LEDBaseString& leds);
        LEDString(const LEDString& leds, uint32_t const offset, const uint32_t length);
        LEDString(const LEDString& other);

        LEDString(CRGB* const leds, const uint32_t total_number_of_leds, uint32_t const offset, const uint32_t length, const bool reversed);
        LEDString(CRGB* const leds, const uint32_t total_number_of_leds, const bool reversed);
        LEDString(const LEDBaseString& leds, uint32_t const offset, const uint32_t length, const bool reversed);
        LEDString(const LEDBaseString& leds, const bool reversed);
        LEDString(const LEDString& leds, uint32_t const offset, const uint32_t length, const bool reversed);
        LEDString(const LEDString& other, const bool reversed);

        void operator=(const LEDString& other);

        CRGB& led(const uint32_t index);
        const CRGB& led(const uint32_t index) const;

        CRGB& operator[](const uint32_t index) {return this->led(index);}
        const CRGB& operator[](const uint32_t index) const {return this->led(index);}

        // Getter
        uint32_t offset() const {return this->base_offset;}
        uint32_t length() const {return this->size;}
        bool is_reversed() const {return this->reversed;}

        // Setter
        LEDString& reverse(const bool direction) {this->reversed = direction; return *this;}
        LEDString& reverse() {this->reversed = !(this->reversed); return *this;}

        // Selbsttest
        LEDString& self_test(const CRGB color, const uint8_t brightness, const uint32_t delay_time_ms);
        LEDString& self_test(const CRGB color, const uint8_t brightness, const bool all_at_once);
        LEDString& self_test(const CHSV color, const uint8_t brightness, const uint32_t delay_time_ms);
        LEDString& self_test(const CHSV color, const uint8_t brightness, const bool all_at_once);
        LEDString& self_test(const CRGB color);
        LEDString& self_test(const CHSV color);
        LEDString& self_test();

        // Einfärben
        LEDString& colorize(const uint32_t index, const CRGB color);
        LEDString& colorize(const uint32_t index, const CHSV color);
        LEDString& colorize(const CRGB color);
        LEDString& colorize(const CHSV color);
        // Einfärben in zwei Abschnitten
        LEDString& colorize(const CRGB color, const uint32_t length, const CRGB background);
        LEDString& colorize(const CRGB color, const uint32_t length, const CHSV background);
        LEDString& colorize(const CHSV color, const uint32_t length, const CRGB background);
        LEDString& colorize(const CHSV color, const uint32_t length, const CHSV background);
        LEDString& colorize(const CRGB color, const uint32_t length);
        LEDString& colorize(const CHSV color, const uint32_t length);

        // Ausschalten
        LEDString& off(const uint32_t index);
        LEDString& off();

        // Sub-Strings erstellen - [start, end)
        LEDString slice(const uint32_t start, const uint32_t end);
        const LEDString slice(const uint32_t start, const uint32_t end) const;

    private:

        uint32_t base_offset;
        uint32_t size;
        bool reversed;

        
};












} // end SoundLight

#endif // LEDSTRING_H

