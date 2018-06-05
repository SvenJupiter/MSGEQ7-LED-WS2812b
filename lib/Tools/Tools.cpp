
#include <Tools.h>
#include "MSGEQ7.h"

namespace SoundLight
{

namespace Tools
{

uint32_t mapInput(uint32_t x, const uint32_t in_min, const uint32_t in_max, const uint32_t out_min, const uint32_t out_max)
{
    if (x < in_min)
        return out_min;
    else if (x > in_max)
        return out_max;
    else
    {
        const uint32_t dy = out_max - out_min;
        const uint32_t dx = in_max - in_min;

        return (((x - in_min) * dy) / dx) + out_min;
    }
}

struct transformation_instructions meqstm = {
    {
        {255, 255, 85, 0, 0, 0, 0},
        {0, 0, 170, 255, 170, 0, 0},
        {0, 0, 0, 0, 85, 255, 255}
    },
    {595, 595, 595}
};

struct transformation_instructions sti = {
    {
        {255, 255, 0, 0, 0, 0, 0},
        {0, 0, 255, 255, 255, 0, 0},
        {0, 0, 0, 0, 0, 255, 255}
    },
    {510, 765, 510}
};


void transform_7_to_3(  const uint8_t input[7], uint8_t output[3], 
                        const struct transformation_instructions* tfi)
{
    if(tfi == standard_transformation_instructions)
        tfi = &sti;

    uint32_t sum = 0;

    for (uint8_t i = 0; i < 3; ++i)
    {
        sum = 0;
        for (uint8_t j = 0; j < 7; ++j)
        {
            sum += ((uint32_t) input[j]) * ((uint32_t) tfi->matrix[i][j]);
        }
        output[i] = (uint8_t) (sum / tfi->divider[i]);
    }
}

void transform_7_to_3(const uint8_t input[7], uint8_t output[3])
{
    output[0] = input[MSGEQ7_LOW];
    output[1] = input[MSGEQ7_MID];
    output[2] = input[MSGEQ7_HIGH];
}


// Blockiert die weitere Abarbeitung des Programmes 
// und läst an <pin> angeschlossene LED mit <frequency> blinken.
// Blinkt mindestens solange wie <duration_ms> und mindestens ein mal.
// Wenn <duration_ms> == 0, dann blinkt unendlich lange
void blink(const uint8_t pin, const uint32_t frequency, uint32_t duration_ms)
{
    pinMode(pin, OUTPUT);
    const uint32_t delay_time_ms = (uint32_t) ((1000.0 / (2 * frequency)) + 0.5);
    const uint32_t start_time = millis();
    do
    {
        digitalWrite(pin, HIGH);
        delay(delay_time_ms);
        digitalWrite(pin, LOW);
        delay(delay_time_ms);
    }
    while(duration_ms == 0 || millis() - start_time <= duration_ms);
    //pinMode(pin, INPUT);
}

// Blockiert die weitere Abarbeitung des Programmes 
// und läst an <pin> angeschlossene LED mit <frequency> blinken
void infinite_blink(const uint8_t pin, const uint32_t frequency)
{
    blink(pin, frequency, 0);
}


void debug(const String text, const bool endl)
{
    if (endl == true)
        Serial.println(text);
    else
        Serial.print(text);
}

bool assert_check(const bool statement, const uint8_t pin, const uint32_t frequency)
{
    if (statement == false)
    {
        infinite_blink(pin, frequency);
    }
    return statement;
}






} // Ende namespace Tools

} // Ende namespace SoundLight

