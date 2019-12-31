#ifndef ArduinoBuiltins_h
#define ArduinoBuiltins_h

#include <stdint.h>

/**
 * This file is meant to replicate the minimum declarations needed
 * to satisfy the samduino library for the arduino API.
 *
 * In testing, these are implemented to provide enough
 * functionality to verify function of this library.
 */

#ifdef __cplusplus
extern "C"
{
#endif

// see: https://www.arduino.cc/reference/en/

/////////
// IO
/////////
#define OUTPUT 0x01
#define INPUT  0x00

#define HIGH 0x01
#define LOW  0x00

void pinMode( uint8_t pin, uint8_t mode );
void digitalWrite( uint8_t pin, uint8_t val );
int digitalRead( uint8_t pin );

/////////
// TIME
/////////
void delay( unsigned long );
void delayMicroseconds( unsigned long );
unsigned long millis();
unsigned long micros();

#ifdef __cplusplus
} // extern "C"
#endif

#endif
