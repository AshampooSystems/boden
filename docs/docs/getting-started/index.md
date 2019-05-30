# Quickstart

## Prerequisites

**On a Mac**: macOS 10.14+, [Xcode 10.1+](https://developer.apple.com/xcode/), [Python 3.4+](https://www.python.org/downloads/), [CMake 3.10.2+](https://cmake.org/download/).

**On Windows**: Windows 10, [Python 3.4+](https://www.python.org/downloads/), [CMake 3.10.2+](https://cmake.org/download/), [Ninja](https://github.com/ninja-build/ninja/releases), [Git](https://git-scm.com/download/win), [Oracle JDK 8](https://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html), and [Android Studio](https://developer.android.com/studio) (with Android NDK, see the [installation instructions](installing_dependencies/windows.md) for further details).

**On Ubuntu 18.04**: `sudo apt update && sudo apt install git cmake python3-distutils openjdk-8-jdk qemu-kvm` plus [Android Studio](https://developer.android.com/studio/) (with Android NDK, see the [installation instructions](installing_dependencies/linux.md) for further details).

## Step 1: Clone the Boden Repo

	git clone --recurse-submodules https://github.com/AshampooSystems/boden.git

## Step 2: Generate and Open an IDE Project

	cd boden
	python boden.py open

This will bring up Xcode on macOS or Android Studio on Linux/Windows.

If anything goes wrong, please make sure that all dependencies are installed and set up correctly. Check out our extended guides for help:

* [Building Boden on macOS](building/mac)
* [Building Boden on Windows](building/windows)
* [Building Boden on Linux](building/linux)

## Step 3: Run a Boden Example Application

In Xcode, select the `bodendemo` target and hit <kbd>Cmd</kbd>+<kbd>R</kbd>.

In Android Studio, select the `bodendemo` target and hit <kbd>Shift</kbd>+<kbd>F10</kbd>.

## Step 4: Create Your First Boden Application

Still with us? Great! Now [learn how to create your first Boden application](first_app.md).