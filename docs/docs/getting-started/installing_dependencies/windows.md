# Setting up Windows for Android builds

## 1. Install Python, CMake, Git, and JDK 8

1. Download and install Python3 from https://www.python.org/downloads/ (ca. 25MB).
	
	> Important: Make sure to activate the “Add Python to PATH" checkbox at the bottom of the installer window.

1. Download and install CMake from https://cmake.org/download/ (ca. 19MB). We recommend using the latest win64-x64 stable release installer binary.

1. Download and install git from https://git-scm.com/download/win (ca. 42MB). If you’re unsure about the available options presented in the installer, just keep the default settings.

	> Important: When installing CMake, make sure to choose “Add the Cmake binary to the PATH environment variable”.

1. Dowload and install JDK 8 from https://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html (ca. 200MB).

## 2. Install Android Studio

### 2.1 Download and Install Android Studio

1. Download Android Studio from https://developer.android.com/studio/ (ca. 1GB) and start the downloaded EXE file.
2. Follow the steps in the setup wizard.
3. Stay on the "Welcome to Android" screen and complete the steps below to complete the installation.

> For Boden, it's sufficient to select the default setup type in the Android Studio Setup wizard. When asked, grant permission to the Intel HAXM installer to allow for Android device emulation.

### 2.2 Install Android NDK

1. On the “Welcome to Android” screen, click Configure and select SDK Manager.
1. Click on the SDK Tools tab.
1. Select NDK from the list and click OK.
1. Confirm the change by clicking OK in the popup window.
1. Accept the NDK license agreement.
1. The NDK component is being installed, grab another drink.
1. After the installation has finished, click Finish. The installation is now complete.
