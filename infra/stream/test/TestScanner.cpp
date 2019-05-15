#include "infra/stream/Scanner.hpp"
#include "gtest/gtest.h"

class ScannerTest : public testing::Test
{
protected:
    template<typename... Args>
    static void CheckFormatArguments(const char* input, const char* format, Args&... args)
    {
        infra::StringInputStream stream(input);
        stream >> infra::Scan(format, args...);
        EXPECT_FALSE(stream.ErrorPolicy().Failed());
    }

    template<class T>
    static void CheckLimits(const char* input, T value)
    {
        T min{};
        T max{};
        CheckFormatArguments(input, "{}..{}", min, max);
        EXPECT_EQ(std::numeric_limits<T>::min(), min);
        EXPECT_EQ(std::numeric_limits<T>::max(), max);
    }

};

TEST_F(ScannerTest, Simple)
{
    uint32_t value{};
    CheckFormatArguments("10", "{}", value);
    EXPECT_EQ(10, value);
}

TEST_F(ScannerTest, SkipLeadingSpaces)
{
    uint32_t value{};
    CheckFormatArguments("  10", "{}", value);
    EXPECT_EQ(10, value);
}

TEST_F(ScannerTest, int_uint8)
{
    CheckLimits("0..255", uint8_t());
}

TEST_F(ScannerTest, int_int8)
{
    CheckLimits("-128..127", int8_t());
}

TEST_F(ScannerTest, int_uint16)
{
    CheckLimits("0..4294967295", uint16_t());
}

TEST_F(ScannerTest, int_int16)
{
    CheckLimits("-32768..32767", int16_t());
}

TEST_F(ScannerTest, int_int32)
{
    CheckLimits("-2147483648..2147483647", int32_t());
}

TEST_F(ScannerTest, int_uint32)
{
    CheckLimits("0..4294967295", uint32_t());
}

TEST_F(ScannerTest, int_uint64)
{
    CheckLimits("0..18446744073709551615", uint64_t());
}

TEST_F(ScannerTest, int_int64)
{
    CheckLimits("-9223372036854775808..9223372036854775807", int64_t());
}
