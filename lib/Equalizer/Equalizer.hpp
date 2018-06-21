// include guard
#pragma once

#include <Equalizer.h>

#define DEBGU false

namespace SoundLight
{

template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
void Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>::
print_equalizer_setting()
{
    for (uint8_t chan = 0; chan < 1 + sizeof...(analogPins); ++chan)
    {
        Serial.println(String(F("Channel ")) + String(chan) + ":");
        Serial.println(String(F("gain: ")) + String(this->eq_setting.channel[chan].gain));
        Serial.println(String(F("agc_amplitude: ")) + String(this->eq_setting.channel[chan].agc_amplitude));
        Serial.println(String(F("agc_enabled: ")) + String(this->eq_setting.channel[chan].agc_enabled));
        
        Serial.print(String(F("EQ-Factors: \t")));
        for (uint8_t freg = 0; freg < 7; ++freg)
        {
            Serial.print(String(this->eq_setting.channel[chan].eq_factor[freg]) + "\t");
        }
        Serial.println();
    }

    Serial.println(String(F("upper_limit: ")) + String(this->eq_setting.upper_limit));
    Serial.println(String(F("lower_limit: ")) + String(this->eq_setting.lower_limit));

}


template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>::
Equalizer(void)
:
    CMSGEQ7<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>()
{
    for (uint8_t chan = 0; chan < 1 + sizeof...(analogPins); ++chan)
    {
        this->eq_setting.channel[chan].gain = Tools::Fraction<uint32_t>(5, 2);
        this->eq_setting.channel[chan].agc_amplitude = Tools::Fraction<uint32_t>(MSGEQ7_OUT_MAX/2);
        this->eq_setting.channel[chan].agc_enabled = false;
        
        for (uint8_t freg = 0; freg < 7; ++freg)
        {
            this->eq_setting.channel[chan].eq_factor[freg] = Tools::Fraction<uint32_t>(1);
        }
    }
    this->eq_setting.upper_limit = MSGEQ7_OUT_MAX;
    this->eq_setting.lower_limit = MSGEQ7_OUT_MIN;

}



template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>::
Equalizer(const struct equalizer_settings eq_setting)
:
    CMSGEQ7<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>(),
    eq_setting(eq_setting)
{}


// functions to read the IC values and save them to the internal array
template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
bool Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>:: 
read(void)
{
    bool result = CMSGEQ7<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>::read();
    if (result)
    {
        eliminateNoise(); // Von Rauschen befreien
        // equalize(); // Equalizer-Settings anwenden
        // normalize(); // Unabhängig von Audiopegel machen
        transform();
    }
    return result;
}
template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
bool Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>:: 
read(const uint32_t interval)
{
    bool result = CMSGEQ7<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>::read(interval);
    if (result)
    {
        eliminateNoise(); // Von Rauschen befreien
        // equalize(); // Equalizer-Settings anwenden
        // normalize(); // Unabhängig von Audiopegel machen
        transform();
    }
    return result;
}




// function for the user to access the values
// frequency
template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
MSGEQ7_data_t Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>:: 
get_equalized_frequency(const uint8_t frequency, const uint8_t channel) const
{
	// dont read out of bounds
	if (frequency >= 7 || channel >= (1 + sizeof...(analogPins)))
		return 0;

	// returns the last measurement
	return equalized_frequencies[frequency].pin[channel];
}
template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
MSGEQ7_data_t Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>:: 
get_equalized_frequency(const uint8_t frequency) const
{
	// dont read out of bounds
	if (frequency >= 7)
		return 0;

	// go through all channels/pins and add measured values
	uint16_t average = 0;
	for (uint8_t p = 0; p < (1 + sizeof...(analogPins)); p++)
		average += equalized_frequencies[frequency].pin[p];

	// return the average of all channels
	return (average / (1 + sizeof...(analogPins)));
}

// function for the user to access the values
// Volume
template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
MSGEQ7_data_t Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>:: 
get_equalized_volume(uint8_t channel) const
{
	// dont read out of bounds
	if (channel >= (1 + sizeof...(analogPins)))
		return 0;

	// add all frequencies of a single channel to the overall volume
	uint16_t vol = 0;
	for (uint8_t i = 0; i < 7; i++)
		vol += get_equalized_frequency(i, channel);

	// return the average of this channels
	return (vol / 7);
}
template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
MSGEQ7_data_t Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>:: 
get_equalized_volume(void) const
{
	// add all frequencies of all channels to the overall volume
	uint16_t vol = 0;
	for (uint8_t i = 0; i < 7; i++)
		vol += get_equalized_frequency(i);

	// return the average of all channels
	return (vol / 7);
}




// function for the user to access the values
// frequency
template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
MSGEQ7_data_t Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>:: 
get_normalized_frequency(const uint8_t frequency, const uint8_t channel) const
{
	// dont read out of bounds
	if (frequency >= 7 || channel >= (1 + sizeof...(analogPins)))
		return 0;

	// returns the last measurement
	return normalized_frequencies[frequency].pin[channel];
}
template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
MSGEQ7_data_t Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>:: 
get_normalized_frequency(const uint8_t frequency) const
{
	// dont read out of bounds
	if (frequency >= 7)
		return 0;

	// go through all channels/pins and add measured values
	uint16_t average = 0;
	for (uint8_t p = 0; p < (1 + sizeof...(analogPins)); p++)
		average += normalized_frequencies[frequency].pin[p];

	// return the average of all channels
	return (average / (1 + sizeof...(analogPins)));
}
// function for the user to access the values
// Volume
template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
MSGEQ7_data_t Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>:: 
get_normalized_volume(uint8_t channel) const
{
	// dont read out of bounds
	if (channel >= (1 + sizeof...(analogPins)))
		return 0;

	// add all frequencies of a single channel to the overall volume
	uint16_t vol = 0;
	for (uint8_t i = 0; i < 7; i++)
		vol += get_normalized_frequency(i, channel);

	// return the average of this channels
	return (vol / 7);
}
template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
MSGEQ7_data_t Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>:: 
get_normalized_volume(void) const
{
	// add all frequencies of all channels to the overall volume
	uint16_t vol = 0;
	for (uint8_t i = 0; i < 7; i++)
		vol += get_normalized_frequency(i);

	// return the average of all channels
	return (vol / 7);
}



// template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
// MSGEQ7_data_t Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>:: 
// get_max_frequency(uint8_t channel) const
// {
//     MSGEQ7_data_t result = get_equalized_frequency(0, channel);
//     MSGEQ7_data_t test;
//     for (uint8_t i = 1; i < 7; ++i)
//     {
//         test = get_equalized_frequency(i, channel);
//         if (test > result)
//             result = test;
//     }
//     return result;

// }
// template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
// MSGEQ7_data_t Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>:: 
// get_max_frequency(void) const
// {
//     MSGEQ7_data_t result = get_equalized_frequency(0);
//     MSGEQ7_data_t test;
//     for (uint8_t i = 1; i < 7; ++i)
//     {
//         test = get_equalized_frequency(i);
//         if (test > result)
//             result = test;
//     }
//     return result;

// }
// template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
// MSGEQ7_data_t Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>:: 
// get_min_frequency(uint8_t channel) const
// {
//     MSGEQ7_data_t result = get_equalized_frequency(0, channel);
//     MSGEQ7_data_t test;
//     for (uint8_t i = 1; i < 7; ++i)
//     {
//         test = get_equalized_frequency(i);
//         if (test < result)
//             result = test;
//     }
//     return result;

// }
// template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
// MSGEQ7_data_t Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>:: 
// get_min_frequency(void) const
// {
//     MSGEQ7_data_t result = get_equalized_frequency(0);
//     MSGEQ7_data_t test;
//     for (uint8_t i = 1; i < 7; ++i)
//     {
//         test = get_equalized_frequency(i);
//         if (test < result)
//             result = test;
//     }
//     return result;

// }



template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
MSGEQ7_data_t Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>:: 
frequency(const uint8_t frequency, const uint8_t channel) const
{
    return this->get_equalized_frequency(frequency, channel);
}
template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
MSGEQ7_data_t Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>:: 
frequency(const uint8_t frequency) const
{
    return this->get_equalized_frequency(frequency);
}
template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
MSGEQ7_data_t Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>:: 
volume(uint8_t channel) const
{
    return this->get_equalized_volume(channel);
}
template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
MSGEQ7_data_t Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>:: 
volume(void) const
{
    return this->get_equalized_volume();
}



// Von Rauschen befreien
template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
void Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>::
eliminateNoise()
{
    // Von Rauschen befreien
    for(uint8_t freq = 0; freq < 7; ++freq)
    {
        for(uint8_t chan = 0; chan < (1 + sizeof...(analogPins)); ++chan)
        {
            this->normalized_frequencies[freq].pin[chan] =  mapNoise(CMSGEQ7<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>::get(freq, chan));
        }
    }
}

  
// // Equalizer-Settings anwenden
// template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
// void Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>::
// equalize()
// {
//     for(uint8_t chan = 0; chan < (1 + sizeof...(analogPins)); ++chan)
//     {
//         for (uint8_t freq = 0; freq < 7; ++freq)
//         {
//             const Tools::Fraction<uint32_t> freg_value(this->equalized_frequencies[freq].pin[chan]);

//             Tools::Fraction<uint32_t> boosted_freg_value = this->eq_setting.eq_factor[freq] * freg_value;

//             this->equalized_frequencies[freq].pin[chan] = 
//                 (MSGEQ7_data_t) Tools::mapInput(boosted_freg_value.value(),
//                     this->eq_setting.lower_limit,
//                     this->eq_setting.upper_limit,
//                     MSGEQ7_OUT_MIN,
//                     MSGEQ7_OUT_MAX);

//         }
//     }
// }


// // Unabhängig von Audiopegel machen
// template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
// void Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>:: 
// normalize()
// {
//     for(uint8_t chan = 0; chan < (1 + sizeof...(analogPins)); ++chan)
//     {
//         if (this->eq_setting.agc_enabled)
//         {
//             const uint32_t scalar = this->get_equalized_volume(chan);
//             if (scalar > 0)
//             {
//                 const Tools::Fraction<uint32_t> agc_gain = this->eq_setting.agc_amplitude / Tools::Fraction<uint32_t>(scalar);

//                 for (uint8_t freq = 0; freq < 7; ++freq)
//                 {
//                     const Tools::Fraction<uint32_t> freq_value(this->equalized_frequencies[freq].pin[chan]); 

//                     this->normalized_frequencies[freq].pin[chan] = 
//                         Tools::mapInput((freq_value * agc_gain).value(),
//                             this->eq_setting.lower_limit,
//                             this->eq_setting.upper_limit,
//                             MSGEQ7_OUT_MIN,
//                             MSGEQ7_OUT_MAX);
//                 }
//             }
//             else
//             {
//                 for (uint8_t freq = 0; freq < 7; ++freq)
//                 {
//                     this->normalized_frequencies[freq].pin[chan] = 0;
//                 }
//             }
//         }
//         else
//         {
//             for (uint8_t freq = 0; freq < 7; ++freq)
//             {
//                 const Tools::Fraction<uint32_t> freq_value(this->equalized_frequencies[freq].pin[chan]);  

//                 this->normalized_frequencies[freq].pin[chan] = 
//                         Tools::mapInput((freq_value * this->eq_setting.gain).value(),
//                             this->eq_setting.lower_limit,
//                             this->eq_setting.upper_limit,
//                             MSGEQ7_OUT_MIN,
//                             MSGEQ7_OUT_MAX);
//             }
//         }
//     }
// }



// Unabhängig von Audiopegel machen
template <uint8_t smooth, uint8_t resetPin, uint8_t strobePin, uint8_t firstAnalogPin, uint8_t ...analogPins>
void Equalizer<smooth, resetPin, strobePin, firstAnalogPin, analogPins ...>:: 
transform()
{
    // Audiosignal verstärken
    Tools::Fraction<uint32_t> gain;
    Tools::Fraction<uint32_t> agc_amplitude; 
    Tools::Fraction<uint32_t> freq_value;
    Tools::Fraction<uint32_t> eq_factor;
    Tools::Fraction<uint32_t> new_freq_value;
    uint32_t volume = 0;

    for(uint8_t chan = 0; chan < (1 + sizeof...(analogPins)); ++chan)
    {
        if (this->eq_setting.channel[chan].agc_enabled)
        {
            
            agc_amplitude = this->eq_setting.channel[chan].agc_amplitude; 
            volume = this->get_normalized_volume(chan);
            gain = (volume != 0 ? agc_amplitude / Tools::Fraction<uint32_t>(volume) : Tools::Fraction<uint32_t>(1));
        }
        else
        {
            gain = this->eq_setting.channel[chan].gain;
        }

        for (uint8_t freq = 0; freq < 7; ++freq)
        {
            freq_value = Tools::Fraction<uint32_t>(this->normalized_frequencies[freq].pin[chan]);
            eq_factor = this->eq_setting.channel[chan].eq_factor[freq];

            new_freq_value = freq_value * gain;
            this->normalized_frequencies[freq].pin[chan] = 
                    Tools::mapInput(new_freq_value.value(),
                        this->eq_setting.lower_limit,
                        this->eq_setting.upper_limit,
                        MSGEQ7_OUT_MIN,
                        MSGEQ7_OUT_MAX);
            
            new_freq_value *= eq_factor;
            this->equalized_frequencies[freq].pin[chan] = 
                    Tools::mapInput(new_freq_value.value(),
                        this->eq_setting.lower_limit,
                        this->eq_setting.upper_limit,
                        MSGEQ7_OUT_MIN,
                        MSGEQ7_OUT_MAX);
        }
    }

}


} // end namespace SoundLight