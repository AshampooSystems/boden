# Setup

You can use macOS to build both iOS and Android apps. Windows and Linux can be used to build Android apps only. This guide will help you set up your system for developing Boden apps.

Which platform do you want to set up for development?

* [macOS](#building-boden-apps-on-macos)
* [Windows](#building-boden-apps-on-windows)
* [Linux](#building-boden-apps-on-linux)

## Building Boden Apps on macOS

Choose your target platform: [iOS](#ios-apps) or [Android](#android-apps).

### iOS Apps

To build iOS apps on macOS, the following dependencies need to be installed:

1. macOS 10.13.6 High Sierra or newer
2. [Xcode 10.1+](https://developer.apple.com/xcode/) (with Command Line Tools installed)
3. [Python 3.4+](https://www.python.org/downloads/)
4. [CMake 3.10.2+](https://cmake.org/download/)

> If you are not sure whether all of the dependencies listed above are installed, jump to the [Dependency Installation Guide for iOS on macOS](#setting-up-macos-for-ios-builds) and follow the steps listed there.

Follow these steps to set up Boden for iOS development:

1. Open the Terminal app and clone the git repository: 

		git clone https://github.com/AshampooSystems/boden.git

3. Run the `boden` build tool to generate an Xcode project:

		cd boden
		git submodule update --init
		./boden prepare -p ios

4. Open the generated project in Xcode:

		open build/ios/std/Xcode/boden.xcodeproj

5. In Xcode select the `uidemo` target and press <kbd>Cmd</kbd>+<kbd>R</kbd> to build and run the example application.

Continue to the [Getting Started](#getting-started) section to learn how to create your first boden app.

### Android Apps

To build Android apps on macOS, the following dependencies need to be installed:

1. macOS 10.13.6 High Sierra or newer
2. Command Line Tools
3. [OpenJDK 8](https://openjdk.java.net/)
4. [Python 3.4+](https://www.python.org/downloads/)
5. [CMake 3.10.2+](https://cmake.org/download/)
6. [Android Studio](https://developer.android.com/studio) (with Android NDK)

> If you are not sure whether all of the dependencies listed above are installed, jump to the [Dependency Installation Guide for Android on macOS](#setting-up-macos-for-android-builds) and follow the steps listed there to install them.

Follow these steps to set up Boden for Android development on the Mac:

1. Open the terminal and clone the git repository:

		git clone https://github.com/AshampooSystems/boden.git

2. Run the `boden` build tool to prepare an Android Studio project:

		cd boden
		git submodule update --init
		./boden prepare -p android

4. Run Android Studio and open `boden/build/android/std/AndroidStudio`.

5. Wait for Android Studio to finish its gradle sync and configuration, then select the `uidemo` target and press <kbd>Ctrl</kbd>+<kbd>R</kbd> to build and run the example application.

Continue to the [Getting Started](#getting-started) section to learn how to create your first boden app.

## Building Boden Apps on Windows

To build Android apps on Windows, the following dependencies need to be installed:

1. Windows 10
2. [Python 3.4+](https://www.python.org/downloads/)
3. [CMake 3.10.2+](https://cmake.org/download/)
4. [Git](https://git-scm.com/download/win)
5. [Oracle JDK 8](https://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html)
6. [Android Studio](https://developer.android.com/studio) (with Android NDK)

> If you are not sure whether all of the dependencies listed above are installed, jump to the [Dependency Installation Guide for Android on Windows](#setting-up-windows-for-android-builds) and follow the steps listed there to install them.

Follow these steps to set up Boden for Android development on Windows 10:

1. Open a Command Prompt: Hit <kbd>Win</kbd>+<kbd>R</kbd>, type `cmd` and then hit Enter.
2. Clone the git repository:

		git clone https://github.com/AshampooSystems/boden.git

3. Run the `boden.py` build tool to generate an Android Studio project:

		cd boden
		git submodule update --init
		python boden.py prepare

4. Run Android Studio and open `boden/build/android/std/AndroidStudio`.

5. Wait for Android Studio to finish its gradle sync and configuration, then select the `uidemo` target and press <kbd>Shift</kbd>+<kbd>F10</kbd> to build and run the example application.

Continue to the [Getting Started](#getting-started) section to learn how to create your first boden app.

### Building Boden Apps on Linux

To build Android apps on Linux, the following dependencies need to be installed:

1. Ubuntu 18.04
2. Git
3. [OpenJDK 8](https://openjdk.java.net/)
4. [Python 3.4+](https://www.python.org/downloads/) (with `python3-distutils`)
5. [CMake 3.10.2+](https://cmake.org/download/)
6. qemu-kvm
6. [Android Studio](https://developer.android.com/studio/) (with Android NDK)

> If you are not sure whether all of the dependencies listed above are installed, jump to the [Dependency Installation Guide for Android on Linux](#setting-up-linux-for-android-builds) and follow the steps listed there to install them.

Follow these steps to set up Boden for Android development on Ubuntu:

1. Open up a terminal and clone the git repository:

		git clone https://github.com/AshampooSystems/boden.git

3. Run the `boden` build tool to generate an Xcode project:

		cd boden
		git submodule update --init
		./boden prepare

4. Run Android Studio and open `boden/build/android/std/AndroidStudio`.

5. Wait for Android Studio to finish its gradle sync and configuration, select the `uidemo` target and press <kbd>Shift</kbd>+<kbd>F10</kbd> to build and run the example application.

> If Android Studio displays an error stating that permission to `/dev/kvm` is denied, make sure that you have added your user to the `kvm` group.
