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

TEST_F(ScannerTest, uint32)
{
    CheckLimits("0..4294967295", uint32_t());
}
