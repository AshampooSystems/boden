# Building on macOS

## iOS Apps

To build iOS apps on macOS, the following dependencies need to be installed:

1. macOS 10.14 Mojave or newer
2. [Xcode 10.1+](https://developer.apple.com/xcode/) (with Command Line Tools installed)
3. [Python 3.4+](https://www.python.org/downloads/)
4. [CMake 3.10.2+](https://cmake.org/download/)

!!! note
	If you are not sure whether all of the dependencies listed above are installed, jump to the [Dependency Installation Guide for iOS on macOS](../dependency_installation/mac.md#setting-up-macos-for-ios-builds) and follow the steps listed there.

Follow these steps to set up Boden for iOS development:

1. Open the Terminal app and clone the git repository: 

		git clone --recurse-submodules https://github.com/AshampooSystems/boden.git

2. Run the `boden` build tool to generate an Xcode project:

		cd boden
		./boden open -p ios

3. In Xcode select the `uidemo` target and press <kbd>Cmd</kbd>+<kbd>R</kbd> to build and run the example application.

Continue to the [Getting Started](#getting-started) section to learn how to create your first boden app.

## Android Apps

To build Android apps on macOS, the following dependencies need to be installed:

1. macOS 10.14 Mojave or newer
2. Command Line Tools
3. [OpenJDK 8](https://openjdk.java.net/)
4. [Python 3.4+](https://www.python.org/downloads/)
5. [CMake 3.10.2+](https://cmake.org/download/)
6. [Android Studio](https://developer.android.com/studio) (with Android NDK)

!!! note
	If you are not sure whether all of the dependencies listed above are installed, jump to the [Dependency Installation Guide for Android on macOS](../dependency_installation/mac.md#setting-up-macos-for-android-builds) and follow the steps listed there to install them.

Follow these steps to set up Boden for Android development on the Mac:

1. Open the terminal and clone the git repository:

		git clone --recurse-submodules https://github.com/AshampooSystems/boden.git

2. Run the `boden` build tool to prepare an Android Studio project:

		cd boden
		./boden open -p android

3. Wait for Android Studio to finish its gradle sync and configuration, then select the `uidemo` target and press <kbd>Ctrl</kbd>+<kbd>R</kbd> to build and run the example application.

Continue to [Your first Application](../first_app.md) to learn how to create your first boden app.
