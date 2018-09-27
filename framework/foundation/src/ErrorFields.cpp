#include <bdn/init.h>
#include <bdn/ErrorFields.h>
#include <bdn/Uri.h>

namespace bdn
{

    ErrorFields::ErrorFields(const String &fieldString)
    {
        if (fieldString.startsWith("[[") && fieldString.endsWith("]]")) {
            auto pos = fieldString.begin() + 2;
            while (true) {
                auto found = fieldString.find(":", pos);
                if (found == fieldString.end())
                    break;

                String name = fieldString.subString(pos, found);
                pos = found;
                ++pos;

                if (*pos == ' ')
                    ++pos;

                if (*pos != '\"')
                    break;

                ++pos;

                found = fieldString.find("\"", pos);
                if (found == fieldString.end())
                    break;

                String value = fieldString.subString(pos, found);
                pos = found;
                ++pos;

                name = unescape(name);
                value = unescape(value);

                add(name, value);

                if (*pos != ',')
                    break;
                ++pos;

                if (*pos == ' ')
                    ++pos;
            }
        }
    }

    String ErrorFields::toString() const
    {
        if (empty())
            return "";
        else {
            String result = "[[";
            bool first = true;

            for (auto item : *this) {
                if (!first)
                    result += ", ";

                result += escapeName(item.first) + ": \"" +
                          escapeValue(item.second) + "\"";

                first = false;
            }

            result += "]]";

            return result;
        }
    }

    String ErrorFields::escapeName(const String &name)
    {
        String result = name;

        result.findAndReplace("%", "%25");
        result.findAndReplace("]]", "%5d%5d");
        result.findAndReplace(":", "%3a");

        return result;
    }

    String ErrorFields::escapeValue(const String &value)
    {
        String result = value;

        result.findAndReplace("%", "%25");
        result.findAndReplace("]]", "%5d%5d");
        result.findAndReplace("\"", "%22");

        return result;
    }

    String ErrorFields::unescape(const String &value)
    {
        return Uri::unescape(value);
    }
}
