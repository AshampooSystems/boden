#pragma once

#include <functional>

#import <UIKit/UIKit.h>

#include <bdn/Application.h>
#include <bdn/log.h>
#include <string>

namespace bdn::ui::ios
{
    bool imageFromUrl(const std::string &url, std::function<void(UIImage *)> callback);
}
