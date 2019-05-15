#include "infra/stream/Scanner.hpp"

namespace infra
{
    ScanSpec::ScanSpec(const char*& format)
        : format(format)
    {
        if (*format != ':')
            return;

        ++format;
        ParseWidth();
        ParseType();
    }

    void ScanSpec::ParseWidth()
    {
        while (isdigit(*format))
            width = 10 * width + *format++ - '0';
    }

    void ScanSpec::ParseType()
    {
        if (*format != '}')
            type = *format++;
    }

    void ScannerBase::SkipWhiteSpace(TextInputStream& stream)
    {
        const auto peek = stream.Reader().PeekContiguousRange(0);
        size_t skipped = 0;
        while (skipped < peek.size() && isspace(peek[skipped]))
            skipped++;
        stream.Consume(skipped);
    }

    bool ScannerBase::IsNegative(TextInputStream& stream)
    {
        const auto peek = stream.Reader().PeekContiguousRange(0);
        if (peek.empty())
            return false;

        const auto c = peek[0];
        if (c != '-' && c != '+')
            return false;

        stream.Consume(1);
        return c == '-';
    }

    uint64_t ScannerBase::RawInteger(TextInputStream& stream)
    {
        const auto peek = stream.Reader().PeekContiguousRange(0);
        size_t index{};

        uint64_t value{};
        for (; index < peek.size(); index++)
        {
            const auto c = peek[index];
            if (!isdigit(c))
                break;
            value = value * 10 + c - '0';
        }

        stream.Consume(index);
        return value;
    }

    int64_t ScannerBase::SignedInteger(TextInputStream& stream)
    {
        SkipWhiteSpace(stream);
        const auto negative = IsNegative(stream);
        const  auto val = RawInteger(stream);
        return negative ? -static_cast<int64_t>(val) : val;
    }

    uint64_t ScannerBase::UnsignedInteger(TextInputStream& stream)
    {
        SkipWhiteSpace(stream);
        return RawInteger(stream);
    }

    template<>
    void Scanner<uint8_t>::Scan(TextInputStream& stream, ScanSpec& spec)
    {
        value = static_cast<uint8_t>(UnsignedInteger(stream));
    }

    template<>
    void Scanner<uint16_t>::Scan(TextInputStream& stream, ScanSpec& spec)
    {
        value = static_cast<uint16_t>(UnsignedInteger(stream));
    }

    template<>
    void Scanner<uint32_t>::Scan(TextInputStream& stream, ScanSpec& spec)
    {
        value = static_cast<uint32_t>(UnsignedInteger(stream));
    }

    template<>
    void Scanner<uint64_t>::Scan(TextInputStream& stream, ScanSpec& spec)
    {
        value = UnsignedInteger(stream);
    }

    template<>
    void Scanner<int8_t>::Scan(TextInputStream& stream, ScanSpec& spec)
    {
        value = static_cast<int8_t>(SignedInteger(stream));;
    }

    template<>
    void Scanner<int16_t>::Scan(TextInputStream& stream, ScanSpec& spec)
    {
        value = static_cast<int16_t>(SignedInteger(stream));;
    }

    template<>
    void Scanner<int32_t>::Scan(TextInputStream& stream, ScanSpec& spec)
    {
        value = static_cast<int32_t>(SignedInteger(stream));;
    }

    template<>
    void Scanner<int64_t>::Scan(TextInputStream& stream, ScanSpec& spec)
    {
        value = SignedInteger(stream);
    }

    ScanWorker::ScanWorker(TextInputStream& stream, const char* formatStr, std::vector<ScannerBase*>& scanners)
        : format(formatStr)
    {
        size_t index = 0;
        do
        {
            if (IsEndFormat())
                return;

            const auto ch = *format++;
            if (ch != '{')
            {
                char c;
                stream >> c;
                if (ch != c)
                    stream.ErrorPolicy().ReportResult(false);
                continue;
            }
            auto spec = ScanSpec(format);

            if (*format++ == '}' && index < scanners.size())
                scanners[index++]->Scan(stream, spec);
            else
                stream.ErrorPolicy().ReportResult(false);
        } while (!stream.Failed());
    }

    bool ScanWorker::IsEndFormat() const
    {
        return *format == '\0';
    }
}
