#include <gtest/gtest.h>

#include "ArduinoTestState.h"
#include "SevenSegment.h"

using namespace samduino;

namespace
{

class SevenSegmentTest : public ::testing::Test
{
public:
    SevenSegmentTest()
    {
        m_state.SetInputOutputProvider( &m_io );

        m_DPins[0] = 10;
        m_DPins[1] = 11;
        m_DPins[2] = 12;
        m_DPins[3] = 13;

        m_layout.PinA = 2;
        m_layout.PinB = 3;
        m_layout.PinC = 4;
        m_layout.PinD = 5;
        m_layout.PinE = 6;
        m_layout.PinF = 7;
        m_layout.PinG = 8;
        m_layout.PinDot = 9;
        m_layout.NumD = 4;
        m_layout.DPins = m_DPins;
        m_layout.DBits = m_DBits;
    }

protected:

    InMemoryInputOutputProvider m_io;
    ArduinoTestState m_state;
    uint8_t m_DPins[4];
    uint8_t m_DBits[4];
    SevenSegmentState m_layout;
};

}

TEST_F( SevenSegmentTest, SetsUp )
{
    SevenSegment seven( m_layout );

    EXPECT_EQ( OUTPUT, m_io.ReadState( m_layout.PinA ).mode );
    EXPECT_EQ( OUTPUT, m_io.ReadState( m_layout.DPins[3] ).mode );
}

TEST_F( SevenSegmentTest, Display_109_1 )
{
    SevenSegment seven( m_layout );

    m_DBits[0] = SevenSegment::MakeBits( 1, Dotted::kWithoutDot );
    m_DBits[1] = SevenSegment::MakeBits( 0, Dotted::kWithoutDot );
    m_DBits[2] = SevenSegment::MakeBits( 9, Dotted::kWithDot );
    m_DBits[3] = SevenSegment::MakeBits( 1, Dotted::kWithoutDot );

    // First digit is a 1
    seven.Display( 0 );
    EXPECT_EQ( LOW, m_io.ReadState( m_layout.PinA ).value );
    EXPECT_EQ( HIGH, m_io.ReadState( m_layout.PinB ).value );

    // Display a 0
    seven.Display( 1 );
    EXPECT_EQ( HIGH, m_io.ReadState( m_layout.PinA ).value );

    // For the 9, make sure the dot is shown
    seven.Display( 2 );
    EXPECT_EQ( HIGH, m_io.ReadState( m_layout.PinA ).value );
    EXPECT_EQ( HIGH, m_io.ReadState( m_layout.PinDot ).value );

    // And for the last digit, no dot and back to a 1
    seven.Display( 3 );
    EXPECT_EQ( LOW, m_io.ReadState( m_layout.PinA ).value );
    EXPECT_EQ( LOW, m_io.ReadState( m_layout.PinDot ).value );
}
