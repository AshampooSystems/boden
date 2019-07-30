#pragma once

#include <functional>

#import <UIKit/UIKit.h>

#include <bdn/Application.h>
#include <bdn/String.h>
#include <bdn/log.h>

namespace bdn::ui::ios
{
    bool imageFromUrl(const String &url, std::function<void(UIImage *)> callback);
}
