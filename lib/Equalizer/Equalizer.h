// Include guard
#pragma once

#include <MSGEQ7.h>
#include <stdint.h>


namespace SoundLight
{

template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
class Equalizer : public CMSGEQ7<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>
{

    public:
        struct equalizer_settings
        {
            Tools::Fraction<uint32_t> eq_factor[7];
            Tools::Fraction<uint32_t> gain;
            Tools::Fraction<uint32_t> agc_amplitude;
            bool agc_enabled;
            uint32_t upper_limit;
            uint32_t lower_limit;
        };

        Equalizer(void);
        Equalizer(const struct equalizer_settings equalizer_setting);

        // functions to read the IC values and save them to the internal array
        bool read(void);
        bool read(const uint32_t interval);


        // function for the user to access the values
        MSGEQ7_data_t get_equalized_frequency(const uint8_t frequency, const uint8_t channel) const;
        MSGEQ7_data_t get_equalized_frequency(const uint8_t frequency) const;
        MSGEQ7_data_t get_equalized_volume(uint8_t channel) const;
        MSGEQ7_data_t get_equalized_volume(void) const;
        MSGEQ7_data_t get_normalized_frequency(const uint8_t frequency, const uint8_t channel) const;
        MSGEQ7_data_t get_normalized_frequency(const uint8_t frequency) const;

        // Kurzer als oben
        MSGEQ7_data_t frequency(const uint8_t frequency, const uint8_t channel) const;
        MSGEQ7_data_t frequency(const uint8_t frequency) const;
        MSGEQ7_data_t volume(uint8_t channel) const;
        MSGEQ7_data_t volume(void) const;

        MSGEQ7_data_t operator[](const uint8_t frequency) const {return this->get_normalized_frequency(frequency);}

    private:

        MSGEQ7_data_t get_max_frequency(uint8_t channel) const;
        MSGEQ7_data_t get_max_frequency(void) const;
        MSGEQ7_data_t get_min_frequency(uint8_t channel) const;
        MSGEQ7_data_t get_min_frequency(void) const;

        void eliminateNoise(); // Von Rauschen befreien
        void equalize(); // Equalizer-Settings anwenden
        void normalize(); // Unabhängig von Audiopegel machen

        struct equalizer_settings equalizer_setting;

        // Array of all input values
        struct frequency{
            MSGEQ7_data_t pin[1 + sizeof...(analogPins)];
        };
        struct frequency equalized_frequencies[7];
        struct frequency normalized_frequencies[7];
 
};












} // end namespace SoundLight

#include <Equalizer.hpp>