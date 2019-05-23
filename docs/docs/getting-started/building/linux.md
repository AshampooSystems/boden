# Building on Linux

To build Android apps on Linux, the following dependencies need to be installed:

1. Ubuntu 18.04
2. Git
3. [OpenJDK 8](https://openjdk.java.net/)
4. [Python 3.4+](https://www.python.org/downloads/) (with `python3-distutils`)
5. [CMake 3.10.2+](https://cmake.org/download/)
6. qemu-kvm
6. [Android Studio](https://developer.android.com/studio/) (with Android NDK)

!!! note
	If you are not sure whether all of the dependencies listed above are installed, jump to the [Dependency Installation Guide for Android on Linux](../installing_dependencies/linux.md#setting-up-linux-for-android-builds) and follow the steps listed there to install them.

Follow these steps to set up Boden for Android development on Ubuntu:

1. Open up a terminal and clone the git repository:

		git clone --recurse-submodules https://github.com/AshampooSystems/boden.git

2. Run the `boden` build tool to generate an Xcode project:

		cd boden
		./boden open -p android

3. Wait for Android Studio to finish its gradle sync and configuration, select the `bodendemo` target and press <kbd>Shift</kbd>+<kbd>F10</kbd> to build and run the example application.

!!! note 
	If Android Studio displays an error stating that permission to `/dev/kvm` is denied, make sure that you have added your user to the `kvm` group.

Continue to [Your first Boden application](../first_app.md) to learn how to create your first Boden app.