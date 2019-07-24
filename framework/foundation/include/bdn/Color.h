#pragma once

#include <any>
#include <array>
#include <limits>
#include <map>
#include <regex>
#include <sstream>

#include <bdn/Json.h>
#include <bdn/String.h>

namespace bdn
{
    class Color
    {
      public:
        Color() = default;
        Color(const Color &other) { _component = other._component; }

        Color(String color);

        constexpr Color(std::array<double, 4> array) { _component = array; }
        constexpr Color(double r, double g, double b, double a = 1.0f) { _component = {r, g, b, a}; }
        constexpr Color(uint32_t color)
        {
            _component = {((color & 0xFF000000) >> 24) / 255.0f, ((color & 0xFF0000) >> 16) / 255.0f,
                          ((color & 0xFF00) >> 8) / 255.0f, (color & 0xFF) / 255.0f};
        }

        static Color fromAny(std::any anyColor);
        static Color fromIntAlphaFirst(uint32_t color)
        {
            Color result{(color & 0xFF) / 255.0f, ((color & 0xFF0000) >> 16) / 255.0f, ((color & 0xFF00) >> 8) / 255.0f,
                         ((color & 0xFF000000) >> 24) / 255.0f};
            return result;
        }

      public:
        inline double red() const { return _component[0]; }
        inline double green() const { return _component[1]; }
        inline double blue() const { return _component[2]; }
        inline double alpha() const { return _component[3]; }

        std::array<double, 4> asArray() const;

        template <class T> std::array<T, 4> asIntArray() const
        {
            std::array<T, 4> intArray;

            std::transform(_component.begin(), _component.end(), intArray.begin(),
                           [](auto c) { return static_cast<T>(std::numeric_limits<T>::max() * c); });

            return intArray;
        }

        uint32_t asInt() const
        {
            auto intArray = asIntArray<uint8_t>();
            return intArray[0] << 24 | intArray[1] << 16 | intArray[2] << 8 | intArray[3];
        }

        uint32_t asIntAlphaFirst() const
        {
            auto intArray = asIntArray<uint8_t>();
            return intArray[3] << 24 | intArray[0] << 16 | intArray[1] << 8 | intArray[2];
        }

      public:
        bool operator==(const Color &other) const { return other._component == this->_component; }
        bool operator!=(const Color &other) const { return other._component != this->_component; }

      private:
        std::array<double, 4> _component = {0.f, 0.f, 0.f, 1.f};

      private:
        static const std::map<String, Color> _namedColors;
    };
}

namespace nlohmann
{
    template <> struct adl_serializer<bdn::Color>
    {
        static void to_json(json &j, const bdn::Color &color)
        {
            j = {color.red(), color.green(), color.blue(), color.alpha()};
        }

        static void from_json(const json &j, bdn::Color &color)
        {
            if (j.is_array()) {
                std::array<double, 4> c = bdn::Color().asArray();
                if (j.size() <= 4) {
                    std::copy_if(j.begin(), j.end(), c.begin(), [](auto j) { return j.is_number(); });
                }
                color = bdn::Color(c);
            } else if (j.is_string()) {
                color = bdn::Color((bdn::String)j);
            }
        }
    };
}
