#pragma once

#include <bdn/java/wrapper/Object.h>

namespace bdn::android::wrapper
{
    constexpr const char kViewParentClassName[] = "android/view/ViewParent";
    using ViewParent = java::wrapper::JTObject<kViewParentClassName>;
}
