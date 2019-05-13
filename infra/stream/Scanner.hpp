#ifndef INFRA_SCANNER_HPP
#define INFRA_SCANNER_HPP

#include "infra/stream/StringInputStream.hpp"

namespace infra
{
    class ScannerHelper
    {
        friend TextInputStream& operator>>(TextInputStream& stream, ScannerHelper&& f)
        {
            char c;
            stream >> c;
            return stream;
        }
    };

    template<class... Args>
    auto Scanner(const char* format, Args&... args)
    {
        return ScannerHelper();
    }

}
#endif
