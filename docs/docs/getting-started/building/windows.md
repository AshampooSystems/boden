# Building on Windows

To build Android apps on Windows, the following dependencies need to be installed:

1. Windows 10
2. [Python 3.4+](https://www.python.org/downloads/)
3. [CMake 3.10.2+](https://cmake.org/download/)
4. [Git](https://git-scm.com/download/win)
5. [Oracle JDK 8](https://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html)
6. [Android Studio](https://developer.android.com/studio) (with Android NDK)

!!! note
	If you are not sure whether all of the dependencies listed above are installed, jump to the [Dependency Installation Guide for Android on Windows](../installing_dependencies/windows.md#setting-up-windows-for-android-builds) and follow the steps listed there to install them.

Follow these steps to set up Boden for Android development on Windows 10:

1. Open a Command Prompt: Hit <kbd>Win</kbd>+<kbd>R</kbd>, type `cmd` and then hit Enter.
2. Clone the git repository:

		git clone --recurse-submodules https://github.com/AshampooSystems/boden.git

3. Run the `boden.py` build tool to generate an Android Studio project:

		cd boden
		python boden.py open -p android

4. Wait for Android Studio to finish its gradle sync and configuration, then select the `uidemo` target and press <kbd>Shift</kbd>+<kbd>F10</kbd> to build and run the example application.

Continue to [Your first Application](../first_app.md) to learn how to create your first boden app.