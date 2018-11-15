#include <bdn/init.h>
#include <bdn/Uri.h>

#include <bdn/hex.h>

namespace bdn
{

    String Uri::unescape(const String &s)
    {
        String result;

        std::string unescapedUtf8;

        auto pos = s.begin();
        auto lastEscapeEnd = s.begin();
        while (true) {
            pos = s.find('%', pos);
            if (pos == s.end() || pos != lastEscapeEnd) {
                if (!unescapedUtf8.empty()) {
                    result += unescapedUtf8;
                    unescapedUtf8.clear();
                }

                result += s.subString(lastEscapeEnd, pos);
                lastEscapeEnd = pos;

                if (pos == s.end())
                    break;
            }

            ++pos;
            if (pos != s.end()) {
                char32_t chr1 = *pos;

                if (isHexDigit(chr1)) {
                    ++pos;

                    if (pos != s.end()) {
                        char32_t chr2 = *pos;
                        if (isHexDigit(chr2)) {
                            ++pos;

                            unescapedUtf8 += (char)((decodeHexDigit(chr1) << 4) | decodeHexDigit(chr2));
                            lastEscapeEnd = pos;
                        }
                    }
                }
            }
        }

        return result;
    }
}
