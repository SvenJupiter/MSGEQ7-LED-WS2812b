#include <LEDString.h>
#include <Tools.h>

namespace SoundLight
{

// -------------- LEDBaseString --------------

LEDBaseString::LEDBaseString(CRGB* const leds, const uint32_t total_number_of_leds)
:
    leds(leds),
    total_number_of_leds(total_number_of_leds)
{}

LEDBaseString::LEDBaseString(const LEDBaseString& other)
:
    leds(other.leds),
    total_number_of_leds(other.total_number_of_leds)
{}

void LEDBaseString::operator=(const LEDBaseString& other)
{
    if (this!=&other) // Vermeide Selbstkopie!
    {
        this->leds = other.leds;
        this->total_number_of_leds = other.total_number_of_leds;
    }
}

// Auf LEDs zugreifen
CRGB& LEDBaseString::led(uint32_t index)
{
    return this->leds[index % this->total_number_of_leds];
}
// Auf LEDs zugreifen
const CRGB& LEDBaseString::led(uint32_t index) const
{
    return this->leds[index % this->total_number_of_leds];
}





// -------------- LEDString --------------

LEDString::LEDString(CRGB* const leds, const uint32_t total_number_of_leds, uint32_t const offset, const uint32_t length)
:
    LEDBaseString(leds, total_number_of_leds),
    base_offset(offset),
    size(length),
    reversed(false)
{}


LEDString::LEDString(CRGB* const leds, const uint32_t total_number_of_leds)
:
    LEDBaseString(leds, total_number_of_leds),
    base_offset(0),
    size(total_number_of_leds),
    reversed(false)
{}


LEDString::LEDString(const LEDBaseString& leds, uint32_t const offset, const uint32_t length)
:
    LEDBaseString(leds),
    base_offset(offset),
    size(length),
    reversed(false)
{}


LEDString::LEDString(const LEDBaseString& leds)
:
    LEDBaseString(leds),
    base_offset(0),
    size(leds.length()),
    reversed(false)
{}


LEDString::LEDString(const LEDString& leds, uint32_t const offset, const uint32_t length)
:
    LEDBaseString(leds),
    base_offset(leds.base_offset + offset),
    size(length),
    reversed(leds.reversed)
{}


LEDString::LEDString(const LEDString& other)
:
    LEDBaseString(other),
    base_offset(other.base_offset),
    size(other.size),
    reversed(other.reversed)
{}








LEDString::LEDString(CRGB* const leds, const uint32_t total_number_of_leds, uint32_t const offset, const uint32_t length, const bool reversed)
:
    LEDBaseString(leds, total_number_of_leds),
    base_offset(offset),
    size(length),
    reversed(reversed)
{}


LEDString::LEDString(CRGB* const leds, const uint32_t total_number_of_leds, const bool reversed)
:
    LEDBaseString(leds, total_number_of_leds),
    base_offset(0),
    size(total_number_of_leds),
    reversed(reversed)
{}


LEDString::LEDString(const LEDBaseString& leds, uint32_t const offset, const uint32_t length, const bool reversed)
:
    LEDBaseString(leds),
    base_offset(offset),
    size(length),
    reversed(reversed)
{}


LEDString::LEDString(const LEDBaseString& leds, const bool reversed)
:
    LEDBaseString(leds),
    base_offset(0),
    size(leds.length()),
    reversed(reversed)
{}


LEDString::LEDString(const LEDString& leds, uint32_t const offset, const uint32_t length, const bool reversed)
:
    LEDBaseString(leds),
    base_offset(leds.base_offset + offset),
    size(length),
    reversed(reversed)
{}

LEDString::LEDString(const LEDString& other, const bool reversed)
:
    LEDBaseString(other),
    base_offset(other.base_offset),
    size(other.size),
    reversed(reversed)
{}


void LEDString::operator=(const LEDString& other)
{
    if (this!=&other) // Vermeide Selbstkopie!
    {
        // Basis-Attribute kopieren
        LEDBaseString::operator=(other);

        // eigene Elemene zuweisen
        this->base_offset = other.base_offset;
        this->size = other.size;
        this->reversed = other.reversed;
    }
}

// Auf LEDs zugreifen
CRGB& LEDString::led(uint32_t index)
{
    index = index % this->size;
    index = (this->reversed ? this->size - 1 - index : index);
    return LEDBaseString::led(this->base_offset + index);
}
// Auf LEDs zugreifen
const CRGB& LEDString::led(uint32_t index) const
{
    index = index % this->size;
    index = (this->reversed ? this->size - 1 - index : index);
    return LEDBaseString::led(this->base_offset + index);
}



// Selbsttest
LEDString& LEDString::self_test(const CRGB color, const uint8_t brightness, const uint32_t delay_time_ms)
{
    // Serial.println(F("SelfTest"));
    FastLED.setBrightness(brightness);

    for (uint32_t i = 0; i < this->length(); ++i)
    {
        this->colorize(i, color);

        if (delay_time_ms > 0)
        {
            FastLED.show();
            delay(delay_time_ms);
        }
    }
    FastLED.show();
    return *this;
}
// Selbsttest
LEDString& LEDString::self_test(const CRGB color, const uint8_t brightness, const bool all_at_once)
{
    const uint32_t delay_ms = 0;
    this->self_test(color, brightness, delay_ms);
    return *this;
}
// Selbsttest
LEDString& LEDString::self_test(const CHSV color, const uint8_t brightness, const uint32_t delay_time_ms)
{
    this->self_test(CRGB(color), brightness, delay_time_ms);
    return *this;
}
// Selbsttest
LEDString& LEDString::self_test(const CHSV color, const uint8_t brightness, const bool all_at_once)
{
    this->self_test(CRGB(color), brightness, all_at_once);
    return *this;
}
// Selbsttest
LEDString& LEDString::self_test(const CRGB color)
{
    const uint8_t brightness = 8;
    const uint32_t delay_ms = 50;
    this->self_test(color, brightness, delay_ms);
    return *this;
}
// Selbsttest
LEDString& LEDString::self_test(const CHSV color)
{
    this->self_test(CRGB(color));
    return *this;
}
// Selbsttest
LEDString& LEDString::self_test()
{
    this->self_test(CRGB::White);
    return *this;
}


// Einfärben
LEDString& LEDString::colorize(const uint32_t index, const CRGB color)
{
    this->led(index) = color;
    return *this;
}
// Einfärben
LEDString& LEDString::colorize(const uint32_t index, const CHSV color)
{
    this->colorize(index, CRGB(color));
    return *this;
}
// Einfärben
LEDString& LEDString::colorize(const CRGB color)
{
    for (uint32_t i = 0; i < this->length(); ++i)
    {
        this->colorize(i, color);
    }
    return *this;
}
// Einfärben
LEDString& LEDString::colorize(const CHSV color)
{
    this->colorize(CRGB(color));
    return *this;
}




// Einfärben in zwei Abschnitten
LEDString& LEDString::colorize(const CRGB color, const uint32_t length, const CRGB background)
{
    for (uint32_t i = 0; i < this->size; ++i)
    {
        this->colorize(i, (i < length ? color : background));
    }
    return *this;
}
// Einfärben in zwei Abschnitten
LEDString& LEDString::colorize(const CRGB color, const uint32_t length, const CHSV background)
{
    return this->colorize(color, length, CRGB(background));
}
// Einfärben in zwei Abschnitten
LEDString& LEDString::colorize(const CHSV color, const uint32_t length, const CRGB background)
{
    return this->colorize(CRGB(color), length, background);
}
// Einfärben in zwei Abschnitten
LEDString& LEDString::colorize(const CHSV color, const uint32_t length, const CHSV background)
{
    return this->colorize(CRGB(color), length, CRGB(background));
}
// Einfärben in zwei Abschnitten
LEDString& LEDString::colorize(const CRGB color, const uint32_t length)
{
    return this->colorize(color, length, CRGB::Black);
}
// Einfärben in zwei Abschnitten
LEDString& LEDString::colorize(const CHSV color, const uint32_t length)
{
    return this->colorize(CRGB(color), length, CRGB::Black);
}




// Ausschalten
LEDString& LEDString::off(const uint32_t index)
{
    this->colorize(index, CRGB(CRGB::Black));
    return *this;
}
// Ausschalten
LEDString& LEDString::off()
{
    this->colorize(CRGB::Black);
    return *this;
}



// Sub-Strings erstellen
LEDString LEDString::slice(const uint32_t start, const uint32_t end)
{
    const uint32_t length = end - start;
    Tools::assert_check(start < end);
    Tools::assert_check(start < this->size);
    Tools::assert_check(end <= this->size);
    if (this->reversed)
    {
        return LEDString(*this, this->size - start - length, length);
    }
    else
    {
        return LEDString(*this, start, length);
    }
}

// Sub-Strings erstellen
const LEDString LEDString::slice(const uint32_t start, const uint32_t end) const
{
    const uint32_t length = end - start;
    Tools::assert_check(start < end);
    Tools::assert_check(start < this->size);
    Tools::assert_check(end <= this->size);
    if (this->reversed)
    {
        return LEDString(*this, this->size - start - length, length);
    }
    else
    {
        return LEDString(*this, start, length);
    }
}












} // end namespace SoundLight