# Setting up Ubuntu Linux for Android Builds

## 1. Install Git, Python, CMake, OpenJDK, and qemu-kvm

Open up a terminal and run the following command:

<pre><code>sudo apt update && sudo apt install git cmake python python3-distutils openjdk-8-jdk qemu-kvm</code></pre>

## 2. Install Android Studio

1. Download Android Studio from https://developer.android.com/studio/ (ca. 1GB) and unpack the downloaded ZIP file.

1. Open up a terminal and change to the directory to which you unpacked the ZIP file. To start Android Studio, run the following commands:

		cd android-studio/bin
		./studio.sh

1. Follow the steps in the setup wizard to complete the installation.

1. If you want to run Android Applications in the Android Emulator, follow the steps below.

## 2. Install Android NDK

1. On the “Welcome to Android” screen, click Configure and select SDK Manager.
1. Click on the SDK Tools tab.
1. Select NDK from the list and click OK.
1. Confirm the change by clicking OK in the popup window.
1. Accept the NDK license agreement.
1. The NDK component is being installed, grab another drink.
1. After the installation has finished, click Finish. The installation is now complete.

!!! note
	In order to run `boden`, the `studio` command-line tool must be installed. Please follow the instructions shown when calling `boden` for the first time after Android Studio has been installed: Open Android Studio, open a project, and then select Tools => Create Command-Line Launcher.

## 3. Configure Ubuntu for Running Android Emulator

1. Open up a terminal and run the following command:

		sudo adduser YOUR_USERNAME kvm

1. Log out of Ubuntu by typing:

		gnome-session-quit

2. Log back in. Android Emulator should now work on your system.

## Didn't work?

Please [let us know](../../../feedback). We're happy to help!
