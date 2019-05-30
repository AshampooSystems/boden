# Troubleshooting

## 1. Android Emulator won't start on Ubuntu Linux due to permission denied error

Make sure that the `qemu-kvm` package is installed and your user is a member of the `kvm` group. See [Configure Ubuntu for Running Android Emulator](../installing_dependencies/linux#3-configure-ubuntu-for-running-android-emulator) for further instructions.

## 2. `boden prepare` fails with a CMake error when setting up a Windows machine for Android development

Double-check whether the right Java version is installed. Android Studio requires the `openjdk-8-jdk` package on Ubuntu Linux or [Oracle JDK 8](https://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html) on Windows. Other Java versions are not supported.

## 3. The Run and Build functions in Android Studio are disabled

Android Studio needs several minutes for its initial preparation (gradle sync, etc.) when the project is first opened. If the build and run functions remain disabled after the preparation has finished please wait an additional minute or two. If they remain disabled then you may be experiencing a sporadically occurring Android Studio bug. Solution: click on the "N processes running" entry in the status bar and then on the play button next to "Indexing paused due to batch update".

## Didn't find what you're looking for?

Please [let us know](../../../feedback). We're happy to help!
