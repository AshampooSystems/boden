
#include <bdn/ErrorFields.h>
#include <bdn/Uri.h>

#include <regex>

namespace bdn
{

    ErrorFields::ErrorFields(const String &fieldString)
    {
        StringView fieldView(fieldString);

        if (cpp20::starts_with(fieldString, "[[") && cpp20::ends_with(fieldString, "]]")) {

            auto pos = size_t{2};
            while (true) {
                auto found = fieldView.find(':', pos);
                if (found == StringView::npos) {
                    break;
                }

                StringView name = fieldView.substr(pos, found - pos);
                pos = found;
                ++pos;

                if (fieldView[pos] == ' ') {
                    ++pos;
                }

                if (fieldView[pos] != '"') {
                    break;
                }

                ++pos;

                found = fieldView.find('"', pos);
                if (found == StringView::npos) {
                    break;
                }

                StringView value = fieldView.substr(pos, found - pos);
                pos = found;
                ++pos;

                add(unescape(String(name)), unescape(String(value)));

                if (fieldView[pos] != ',') {
                    break;
                }
                ++pos;

                if (fieldView[pos] == ' ') {
                    ++pos;
                }
            }
        }
    }

    String ErrorFields::toString() const
    {
        if (empty()) {
            return "";
        }
        String result = "[[";
        bool first = true;

        for (const auto &item : *this) {
            if (!first) {
                result += ", ";
            }

            result += escapeName(item.first) + ": \"" + escapeValue(item.second) + "\"";

            first = false;
        }

        result += "]]";

        return result;
    }

    String ErrorFields::escapeName(const String &name)
    {
        String result = std::regex_replace(name, std::regex("%"), "%25");
        result = std::regex_replace(result, std::regex("\\]\\]"), "%5d%5d");
        result = std::regex_replace(result, std::regex(":"), "%3a");

        return result;
    }

    String ErrorFields::escapeValue(const String &value)
    {
        String result = std::regex_replace(value, std::regex("%"), "%25");
        result = std::regex_replace(result, std::regex("\\]\\]"), "%5d%5d");
        result = std::regex_replace(result, std::regex("\\\\"), "%22");

        return result;
    }

    String ErrorFields::unescape(const String &value) { return Uri::unescape(value); }
}
