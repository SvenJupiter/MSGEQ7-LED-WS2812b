#ifndef TOOLS_H
#define TOOLS_H

#include <Arduino.h>
#include <stdint.h>

namespace SoundLight
{

namespace Tools
{

template<typename T>
void swap(T& a, T& b)
{
    const T temp = a;
    a = b;
    b = temp;
}

template< typename A, typename B>
B mapInput(A x, const A in_min, const A in_max, const B out_min, const B out_max)
{
    if (x < in_min)
        return out_min;
    else if (x > in_max)
        return out_max;
    else
    {
        const B dy = out_max - out_min;
        const B dx = in_max - in_min;

        return (((x - in_min) * dy) / dx) + out_min;
    }
}

uint32_t mapInput(uint32_t x, const uint32_t in_min, const uint32_t in_max, const uint32_t out_min, const uint32_t out_max);


struct transformation_instructions
{
    uint8_t matrix[3][7];
    uint32_t divider[3];
};

struct transformation_instructions* const standard_transformation_instructions = nullptr;

void transform_7_to_3(  const uint8_t input[7], uint8_t output[3], 
                        const struct transformation_instructions* tfi);


void transform_7_to_3(const uint8_t input[7], uint8_t output[3]);

// Blockiert die weitere Abarbeitung des Programmes 
// und läst an <pin> angeschlossene LED mit <frequency> blinken.
// Blinkt mindestens solange wie <duration_ms> und mindestens ein mal.
// Wenn <duration_ms> == 0, dann blinkt unendlich lange
void blink(const uint8_t pin = 13, const uint32_t frequency = 5, uint32_t duration_ms = 0);
void infinite_blink(const uint8_t pin = 13, const uint32_t frequency = 5);




void debug(const String text, const bool endl = true);

bool assert_check(const bool statement = true, const uint8_t pin = 13, const uint32_t frequency = 5);




template<typename T>
class Fraction
{
    private:

        T numerator;
        T denominator;
        T ggt(T x, T y) const
        { /* gibt ggt(x,y) zurueck, falls x oder y nicht 0 */
            T c;                /* und gibt 0 zurueck fuer x=y=0.   */
            if ( x < 0 ) x = -x;  
            if ( y < 0 ) y = -y;
            while ( y != 0 )
            {
                /* solange y != 0 */
                c = x % y; x = y; y = c; 
                /* ersetze x durch y und
                y durch den Rest von x modulo y */
            }
            return x;
        }
        void simplify()
        {
            const T ggt_value = ggt(this->numerator, this->denominator);
            this->numerator /= ggt_value;
            this->denominator /= ggt_value;
        }

        

    public:

        Fraction()
        :
            numerator(0),
            denominator(1)
        {}
        Fraction(const T numerator)
        :
            numerator(numerator),
            denominator(1)
        {}
        Fraction(const T numerator, const T denominator)
        :
            numerator(numerator),
            denominator(denominator)
        {
            assert_check(denominator != (T)0); 
            if(this->denominator < 0)
            {
                this->numerator *= (T) -1;
                this->denominator *= (T) -1;
            }
            simplify();
        }
        Fraction(const Fraction& other)
        :
            numerator(other.numerator),
            denominator(other.denominator)
        {}
        void operator=(const Fraction& other)
        {
            this->numerator = other.numerator;
            this->denominator = other.denominator;
        }
        
        Fraction<T> reciprocal() const {return Fraction<T>(this->denominator, this->numerator);}
        Fraction<T> flip() const {return this->reciprocal();}
        Fraction<T> invert() const {return Fraction<T>((T) -1 * this->numerator, this->denominator);}
        T value() const {return this->numerator / this->denominator;}
        // operator T() const {return this->value();}
        String to_str() const {return String('(') + String(this->numerator) + String('/') + String(this->denominator) + String(')');}
        // operator String() const {return this->to_str();}
        // explicit operator String() const {return this->to_str();}


        
        Fraction<T> operator+(const Fraction<T> other) const
        {
            const T num = this->numerator * other.denominator + other.numerator * this->denominator;
            const T den = this->denominator * other.denominator;
            return Fraction<T>(num, den);
        }
        Fraction<T> operator-(const Fraction<T> other) const
        {
            const T num = this->numerator * other.denominator - other.numerator * this->denominator;
            const T den = this->denominator * other.denominator;
            return Fraction<T>(num, den);
        }
        Fraction<T> operator*(const Fraction<T> other) const
        {
            const T num = this->numerator * other.numerator;
            const T den = this->denominator * other.denominator;
            return Fraction<T>(num, den);
        }
        Fraction<T> operator/(const Fraction<T> other) const
        {
            const T num = this->numerator * other.denominator;
            const T den = this->denominator * other.numerator;
            return Fraction<T>(num, den);
        }
        Fraction<T> operator+=(const Fraction<T> other)
        {
            const T num = this->numerator * other.denominator + other.numerator * this->denominator;
            const T den = this->denominator * other.denominator;
            this->numerator = num;
            this->denominator = den;
            simplify();
        }
        Fraction<T> operator-=(const Fraction<T> other)
        {
            const T num = this->numerator * other.denominator - other.numerator * this->denominator;
            const T den = this->denominator * other.denominator;
            this->numerator = num;
            this->denominator = den;
            simplify();
        }
        Fraction<T> operator*=(const Fraction<T> other)
        {
            const T num = this->numerator * other.numerator;
            const T den = this->denominator * other.denominator;
            this->numerator = num;
            this->denominator = den;
            simplify();
        }
        Fraction<T> operator/=(const Fraction<T> other)
        {
            const T num = this->numerator * other.denominator;
            const T den = this->denominator * other.numerator;
            this->numerator = num;
            this->denominator = den;
            simplify();
        }


        // Boolsche Operatoren
        operator bool() const {return this->numerator != (T) 0;}
        bool operator!() const {return this->numerator == (T) 0;}
        bool operator>(const Fraction<T> other) const 
        {
            return this->numerator * other.denominator > other.numerator * this->denominator;
        }
        bool operator>=(const Fraction<T> other) const 
        {
            return this->numerator * other.denominator >= other.numerator * this->denominator; 
        }
        bool operator<(const Fraction<T> other) const 
        {
            return this->numerator * other.denominator < other.numerator * this->denominator; 
        }
        bool operator<=(const Fraction<T> other) const 
        {
            return this->numerator * other.denominator <= other.numerator * this->denominator; 
        }
        bool operator==(const Fraction<T> other) const 
        {
            return this->numerator * other.denominator == other.numerator * this->denominator; 
        }
        bool operator!=(const Fraction<T> other) const 
        {
            return this->numerator * other.denominator != other.numerator * this->denominator; 
        }

};


} // Ende namespace Tools

} // Ende namespace SoundLight




#endif // TOOLS_H