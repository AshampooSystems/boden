#pragma once

#include <bdn/android/wrapper/Context.h>
#include <bdn/android/wrapper/Window.h>

namespace bdn::android::wrapper
{
    constexpr const char kActivityClassName[] = "android/app/Activity";

    class Activity : public BaseContext<kActivityClassName>
    {
      public:
        using BaseContext<kActivityClassName>::BaseContext;

      public:
        JavaMethod<Window()> getWindow{this, "getWindow"};
        JavaMethod<void(int)> setRequestedOrientation{this, "setRequestedOrientation"};
    };

    constexpr const char kActivityInfoClassName[] = "android/content/pm/ActivityInfo";
    class ActivityInfo : public java::wrapper::JTObject<kActivityInfoClassName>
    {
      public:
        using JTObject<kActivityInfoClassName>::JTObject;

      public:
        constexpr static java::StaticFinalField<int, ActivityInfo> SCREEN_ORIENTATION_UNSET{"SCREEN_ORIENTATION_UNSET"};
        constexpr static java::StaticFinalField<int, ActivityInfo> SCREEN_ORIENTATION_UNSPECIFIED{
            "SCREEN_ORIENTATION_UNSPECIFIED"};
        constexpr static java::StaticFinalField<int, ActivityInfo> SCREEN_ORIENTATION_LANDSCAPE{
            "SCREEN_ORIENTATION_LANDSCAPE"};
        constexpr static java::StaticFinalField<int, ActivityInfo> SCREEN_ORIENTATION_PORTRAIT{
            "SCREEN_ORIENTATION_PORTRAIT"};
        constexpr static java::StaticFinalField<int, ActivityInfo> SCREEN_ORIENTATION_USER{"SCREEN_ORIENTATION_USER"};
        constexpr static java::StaticFinalField<int, ActivityInfo> SCREEN_ORIENTATION_BEHIND{
            "SCREEN_ORIENTATION_BEHIND"};
        constexpr static java::StaticFinalField<int, ActivityInfo> SCREEN_ORIENTATION_SENSOR{
            "SCREEN_ORIENTATION_SENSOR"};
        constexpr static java::StaticFinalField<int, ActivityInfo> SCREEN_ORIENTATION_NOSENSOR{
            "SCREEN_ORIENTATION_NOSENSOR"};
        constexpr static java::StaticFinalField<int, ActivityInfo> SCREEN_ORIENTATION_SENSOR_LANDSCAPE{
            "SCREEN_ORIENTATION_SENSOR_LANDSCAPE"};
        constexpr static java::StaticFinalField<int, ActivityInfo> SCREEN_ORIENTATION_SENSOR_PORTRAIT{
            "SCREEN_ORIENTATION_SENSOR_PORTRAIT"};
        constexpr static java::StaticFinalField<int, ActivityInfo> SCREEN_ORIENTATION_REVERSE_LANDSCAPE{
            "SCREEN_ORIENTATION_REVERSE_LANDSCAPE"};
        constexpr static java::StaticFinalField<int, ActivityInfo> SCREEN_ORIENTATION_REVERSE_PORTRAIT{
            "SCREEN_ORIENTATION_REVERSE_PORTRAIT"};
        constexpr static java::StaticFinalField<int, ActivityInfo> SCREEN_ORIENTATION_FULL_SENSOR{
            "SCREEN_ORIENTATION_FULL_SENSOR"};
        constexpr static java::StaticFinalField<int, ActivityInfo> SCREEN_ORIENTATION_USER_LANDSCAPE{
            "SCREEN_ORIENTATION_USER_LANDSCAPE"};
        constexpr static java::StaticFinalField<int, ActivityInfo> SCREEN_ORIENTATION_USER_PORTRAIT{
            "SCREEN_ORIENTATION_USER_PORTRAIT"};
        constexpr static java::StaticFinalField<int, ActivityInfo> SCREEN_ORIENTATION_FULL_USER{
            "SCREEN_ORIENTATION_FULL_USER"};
        constexpr static java::StaticFinalField<int, ActivityInfo> SCREEN_ORIENTATION_LOCKED{
            "SCREEN_ORIENTATION_LOCKED"};
    };
}
