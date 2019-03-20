#pragma once

#include <bdn/String.h>
#include <bdn/ViewCore.h>
#include <bdn/Window.h>

#include <sstream>

namespace bdn
{
    /** The core for a top level window.*/
    class WindowCore
    {
      public:
        enum Orientation : int
        {
            Portrait = 0x1,
            LandscapeLeft = 0x2,
            LandscapeRight = 0x4,
            PortraitUpsideDown = 0x8,
            LandscapeMask = (LandscapeLeft | LandscapeRight),
            PortraitMask = (Portrait | PortraitUpsideDown),
            All = (LandscapeMask | PortraitMask)
        };

        // TODO: Move to .cpp
        static String orientationToString(Orientation orientation)
        {
            using namespace std::string_literals;
            std::stringstream str;
            if (orientation & Orientation::Portrait) {
                str << "Portrait"s;
            }
            if (orientation & Orientation::LandscapeLeft) {
                if (str.tellp() != 0) {
                    str << " | ";
                }
                str << "LandscapeLeft"s;
            }
            if (orientation & Orientation::LandscapeRight) {
                if (str.tellp() != 0) {
                    str << " | ";
                }
                str << "LandscapeRight"s;
            }
            if (orientation & Orientation::PortraitUpsideDown) {
                if (str.tellp() != 0) {
                    str << " | ";
                }
                str << "PortraitUpsideDown"s;
            }
            return str.str();
        }

      public:
        Property<std::shared_ptr<View>> content;
        Property<Rect> contentGeometry;
        Property<String> title;

        Property<Orientation> allowedOrientations;
        Property<Orientation> currentOrientation;
    };
}
