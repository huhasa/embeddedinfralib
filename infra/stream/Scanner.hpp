#ifndef INFRA_SCANNER_HPP
#define INFRA_SCANNER_HPP

#include "infra/stream/StringInputStream.hpp"

namespace infra
{
    struct ScanSpec
    {
        explicit ScanSpec(const char*& format);

        std::size_t width{ 0 };
        char type{ '\0' };

    private:
        void ParseWidth();
        void ParseType();

    private:
        const char*& format;
    };

    class ScannerBase
    {
    public:
        virtual ~ScannerBase() = default;
        virtual void Scan(TextInputStream& stream, ScanSpec& spec) = 0;

    protected:
        static int64_t SignedInteger(TextInputStream& stream);
        static uint64_t UnsignedInteger(TextInputStream& stream);

    private:
        static void SkipWhiteSpace(TextInputStream& stream);
        static bool IsNegative(TextInputStream& stream);
        static uint64_t RawInteger(TextInputStream& stream);
    };

    template<typename T>
    class Scanner final
        : public ScannerBase
    {
    public:
        explicit Scanner(T& value);

        void Scan(TextInputStream& stream, ScanSpec& spec) override;

    private:
        T& value;
    };

    template <typename T>
    Scanner<T>::Scanner(T& value)
        : value(value)
    {}

    template<>
    void Scanner<uint8_t>::Scan(TextInputStream& stream, ScanSpec& spec);

    template<>
    void Scanner<uint16_t>::Scan(TextInputStream& stream, ScanSpec& spec);

    template<>
    void Scanner<uint32_t>::Scan(TextInputStream& stream, ScanSpec& spec);

    template<>
    void Scanner<uint64_t>::Scan(TextInputStream& stream, ScanSpec& spec);

    template<>
    void Scanner<int8_t>::Scan(TextInputStream& stream, ScanSpec& spec);

    template<>
    void Scanner<int16_t>::Scan(TextInputStream& stream, ScanSpec& spec);

    template<>
    void Scanner<int32_t>::Scan(TextInputStream& stream, ScanSpec& spec);

    template<>
    void Scanner<int64_t>::Scan(TextInputStream& stream, ScanSpec& spec);

    template<class T>
    auto MakeScanner(T& v)
    {
        return Scanner<T>(v);
    }

    class ScanWorker
    {
    public:
        explicit ScanWorker(TextInputStream& stream, const char* formatStr, std::vector<ScannerBase*>& scanners);

    private:
        bool IsEndFormat() const;

    private:
        const char* format;
    };

    template<class... Args>
    class ScannerHelper
    {
    public:
        explicit ScannerHelper(const char* format, Args... args)
            : format(format)
            , args(std::forward<Args>(args)...)
            , scanners{ MakeScanner() }
        {}

        friend TextInputStream& operator>>(TextInputStream& stream, ScannerHelper&& s)
        {
            ScanWorker(stream, s.format, s.scanners);
            return stream;
        }

    private:
        template<std::size_t... Is>
        std::vector<ScannerBase*> Make(std::index_sequence<Is...>)
        {
            return{ &std::get<Is>(args)... };
        }

        std::vector<ScannerBase*> MakeScanner()
        {
            return Make(std::index_sequence_for<Args...>{});
        };


        const char* format;
        std::tuple<Args ...> args;
        std::vector<ScannerBase*> scanners{ sizeof...(Args), nullptr };
        uint32_t autoIndex{};

    };

    template<class... Args>
    auto Scan(const char* format, Args&... args)
    {
        return ScannerHelper<Scanner<Args>...>(format, MakeScanner(args)...);
    }

}
#endif
