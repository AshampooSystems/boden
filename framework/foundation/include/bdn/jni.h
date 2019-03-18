#pragma once

#include <bdn/platform.h>

#ifdef BDN_PLATFORM_ANDROID

#ifdef __ANDROID__
#include_next <jni.h>
#else
#include "fake_jni/jni.h"
#endif

#endif
