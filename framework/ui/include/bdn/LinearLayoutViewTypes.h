#pragma once

#include <bdn/Margin.h>
#include <bdn/Point.h>
#include <bdn/Rect.h>
#include <bdn/Size.h>

#include <bdn/View.h>

namespace bdn
{
    class VirtualPoint : Point
    {
      public:
        VirtualPoint(bool h) : primary(h ? x : y), secondary(h ? y : x) {}

        VirtualPoint(bool h, double p, double s) : primary(h ? x : y), secondary(h ? y : x)
        {
            primary = p;
            secondary = s;
        }

        Point toPoint() { return *this; }

        double &primary;
        double &secondary;
    };

    class VirtualMargin : Margin
    {
      public:
        VirtualMargin(bool h)
            : primaryNear(h ? left : top), primaryFar(h ? right : bottom), secondaryNear(h ? top : left),
              secondaryFar(h ? bottom : right)
        {}

        VirtualMargin(bool h, const Margin &other) : VirtualMargin(h) { Margin::operator=(other); }

        Margin toMargin() const { return *this; }

        double &primaryNear;
        double &primaryFar;
        double &secondaryNear;
        double &secondaryFar;
    };

    class VirtualSize : Size
    {
      public:
        VirtualSize(bool h) : primary(h ? width : height), secondary(h ? height : width) {}

        VirtualSize(bool h, const Size &other) : primary(h ? width : height), secondary(h ? height : width)
        {
            Size::operator=(other);
        }

        VirtualSize(bool h, double p, double s) : primary(h ? width : height), secondary(h ? height : width)
        {
            primary = p;
            secondary = s;
        }

        void applyMaximum(const VirtualSize &size) { Size::applyMaximum(size); }
        void applyMaximum(const Size &size) { Size::applyMaximum(size); }
        void applyMinimum(const Size &size) { Size::applyMinimum(size); }

        Size toSize() { return *this; }

        double &primary;
        double &secondary;
    };

    class VirtualRect : Rect
    {
      public:
        VirtualRect(bool h, const Rect &other)
            : _h(h), primary(h ? x : y), secondary(h ? y : x), primarySize(h ? width : height),
              secondarySize(h ? height : width)
        {
            Rect::operator=(other);
        }

        VirtualRect(const VirtualRect &other)
            : _h(other._h), primary(_h ? x : y), secondary(_h ? y : x), primarySize(_h ? width : height),
              secondarySize(_h ? height : width)
        {
            Rect::operator=(other.toRect());
        }

        Rect toRect() const { return *this; }

        /** Increase the rect size by adding the specified margin.*/
        VirtualRect operator+(const VirtualMargin &margin) const { return VirtualRect(*this) += margin; }

        VirtualRect &operator=(const VirtualRect &other)
        {
            Rect::operator=(other);
            return *this;
        }

        /** Increase the rect size by adding the specified margin.*/
        VirtualRect &operator+=(const VirtualMargin &margin)
        {
            primary -= margin.primaryNear;
            secondary -= margin.secondaryNear;
            primarySize += margin.primaryNear + margin.primaryFar;
            secondarySize += margin.secondaryNear + margin.secondaryFar;

            return *this;
        }

        /** Returns the size of the rect.*/
        VirtualSize getSize() const { return VirtualSize(_h, primary, secondary); }

      private:
        bool _h;

      public:
        double &primary;
        double &secondary;
        double &primarySize;
        double &secondarySize;
    };

    enum VirtualAlignment
    {
        near,
        middle,
        far,
        expand
    };

    inline VirtualAlignment primaryToVirtualAlignment(bool h, const std::shared_ptr<View> &view)
    {
        if (h) {
            return (VirtualAlignment)*view->horizontalAlignment;
        } else {
            return (VirtualAlignment)*view->verticalAlignment;
        }
    }

    inline VirtualAlignment secondaryToVirtualAlignment(bool h, const std::shared_ptr<View> &view)
    {
        if (h) {
            return (VirtualAlignment)*view->verticalAlignment;
        } else {
            return (VirtualAlignment)*view->horizontalAlignment;
        }
    }
}