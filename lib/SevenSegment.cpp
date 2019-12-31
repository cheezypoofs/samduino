#include "SevenSegment.h"
#include "Arduino.h"
#include "Scheduler.h"

namespace samduino
{

namespace
{
                                // abcd efg.
const uint8_t kDigit0 = 0xFC;   // 1111 1100
const uint8_t kDigit1 = 0x60;   // 0110 0000
const uint8_t kDigit2 = 0xDA;   // 1101 1010
const uint8_t kDigit3 = 0xF2;   // 1111 0010
const uint8_t kDigit4 = 0x66;   // 0110 0110
const uint8_t kDigit5 = 0xB6;   // 1011 0110
const uint8_t kDigit6 = 0xBE;   // 1011 1110
const uint8_t kDigit7 = 0xE0;   // 1110 0000
const uint8_t kDigit8 = 0xFE;   // 1111 1110
const uint8_t kDigit9 = 0xF6;   // 1111 0110
}

SevenSegment::SevenSegment( SevenSegmentState& state )
    : m_state( state )
    , m_displayWork( nullptr )
{
    const uint8_t* pins = &m_state.PinA;
    for ( uint8_t i = 0; i < 8; i++ )
    {
        const uint8_t pin = pins[i];
        pinMode( pin, OUTPUT );
    }

    if ( m_state.DPins )
    {
        for ( uint8_t i = 0; i < m_state.NumD; i++ )
        {
            const uint8_t pin = m_state.DPins[i];
            pinMode( pin, OUTPUT );
        }
    }
}

void SevenSegment::Display( uint8_t which )
{
    // Pull them all up so they are off while we change
    // Pull down the selection pin (this enables it)
    if ( m_state.DPins )
    {
        for ( uint8_t i = 0; i < m_state.NumD; i++ )
        {
            const uint8_t pin = m_state.DPins[i];
            digitalWrite( pin, HIGH );
        }
    }

    // And then set the 7 segment to the bits requested
    if ( m_state.DBits )
    {
        const uint8_t* pins = &m_state.PinA;
        uint8_t mask = SEVEN_SEGMENT_BIT_A_MASK;

        for ( uint8_t i = 0; i < 8; i++, mask = mask >> 1 )
        {
            const uint8_t pin = pins[i];
            const uint8_t bits = m_state.DBits[which];

            const uint8_t val = ( bits & mask ) ? HIGH : LOW;
            digitalWrite( pin, val );
        }
    }

    // Pull down the selection pin (this enables it)
    if ( m_state.DPins && which < m_state.NumD )
    {
        digitalWrite( m_state.DPins[which], LOW );
    }
}

uint8_t SevenSegment::MakeBits( uint8_t value, Dotted dotted )
{
    uint8_t result = 0;

    switch ( value )
    {
    case 0:
        result = kDigit0;
        break;
    case 1:
        result = kDigit1;
        break;
    case 2:
        result = kDigit2;
        break;
    case 3:
        result = kDigit3;
        break;
    case 4:
        result = kDigit4;
        break;
    case 5:
        result = kDigit5;
        break;
    case 6:
        result = kDigit6;
        break;
    case 7:
        result = kDigit7;
        break;
    case 8:
        result = kDigit8;
        break;
    case 9:
        result = kDigit9;
        break;

    // Could add letters too
    case 'E':
        result = 0x9E;
        break;
    case 'o':
        result = 0x3A;
        break;
    case 'r':
        result = 0x0A;
        break;
    case ' ':
        result = 0x00;
        break;
    }

    if ( result && dotted == Dotted::kWithDot )
    {
        result |= SEVEN_SEGMENT_BIT_DOT_MASK;
    }

    return result;
}

void SevenSegment::SetError()
{
    if ( !m_state.DBits )
    {
        return;
    }

    for ( uint8_t i = 0; i < m_state.NumD; i++ )
    {
        switch ( i )
        {
        case 0:
            m_state.DBits[i] = MakeBits( 'E' );
            break;
        case 1:
        case 2:
        case 4:
            m_state.DBits[i] = MakeBits( 'r' );
            break;
        case 3:
            m_state.DBits[i] = MakeBits( 'o' );
            break;
        default:
            m_state.DBits[i] = MakeBits( ' ' );
            break;
        }
    }
}


////////////////

SevenSegmentDisplayWork::SevenSegmentDisplayWork( SevenSegment& seven )
    : m_7( seven )
    , m_nextRun( 0 )
    , m_which( 0 )
{}

unsigned long SevenSegmentDisplayWork::DueAtMillis()
{
    return m_nextRun;
}

void SevenSegmentDisplayWork::DoWork()
{
    uint8_t which = ( m_which + 1 ) % m_7.State().NumD;
    m_7.Display( which );

    m_which = which;
    m_nextRun = millis() + 5;
}

} // samduino
