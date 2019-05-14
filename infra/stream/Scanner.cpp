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

    template<>
    void Scanner<uint32_t>::Scan(TextInputStream& stream, ScanSpec& spec)
    {
        const auto peek = stream.Reader().PeekContiguousRange(0);
        size_t index = 0;
        while (index < peek.size())
        {
            const auto c = peek[index];
            if (!isdigit(c))
                break;
            value = value * 10 + c - '0';
            index++;
        }
        stream.Consume(index);
    }

    ScanWorker::ScanWorker(TextInputStream& stream, const char* formatStr, std::vector<ScannerBase*>& scanners)
        : format(formatStr)
    {
        do
        {
            if (IsEndFormat())
                return;

            const auto ch = *format++;
            if (ch != '{')
            {
                char c;
                stream >> c;
                continue;
            }
            const auto index = ParseIndex();
            auto spec = ScanSpec(format);

            if (*format++ == '}' && index < scanners.size())
                scanners[index]->Scan(stream, spec);
            else
                stream.ErrorPolicy().ReportResult(false);
        } while (!stream.Failed());
    }

    bool ScanWorker::IsEndFormat() const
    {
        return *format == '\0';
    }

    uint32_t ScanWorker::ParseIndex()
    {
        if (!isdigit(*format))
            return autoIndex++;

        uint32_t index = 0;
        do
        {
            index = 10 * index + *format++ - '0';
        } while (isdigit(*format));

        return index;
    }

}
