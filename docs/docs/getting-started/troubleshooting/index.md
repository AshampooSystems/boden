# Troubleshooting

## 1. Android Emulator won't start on Ubuntu Linux due to permission denied error

Make sure that the `qemu-kvm` package is installed and your user is a member of the `kvm` group. See [Configure Ubuntu for Running Android Emulator](../installing_dependencies/linux#3-configure-ubuntu-for-running-android-emulator) for further instructions.

## 2. `boden prepare` fails with a CMake error when setting up a Windows machine for Android development

Double-check whether the right Java version is installed. Android Studio requires the `openjdk-8-jdk` package on Ubuntu Linux or [Oracle JDK 8](https://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html) on Windows. Other Java versions are not supported.

## 3. The Run and Build functions in Android Studio are disabled

Android Studio may need up to several minutes for its initial preparation (gradle sync, etc.) when the project is first opened. If the build and run functions remain disabled after the preparation has finished, please wait an additional minute or two. If they still remain disabled, you may be experiencing a sporadically occurring Android Studio bug. Solution: click on the "N processes running" entry in the status bar and then on the play button next to "Indexing paused due to batch update".

## 4. boden.py script runs into SSL certificate exception when using Python 3 from python.org on macOS

If you're using Python 3, you may run into an issue with SSL certificates on macOS when running `boden.py` or `boden`.

We recommend [installing Python 3 via Homebrew](../installing_dependencies/mac-ios/#2-install-homebrew-python-and-cmake) which should correctly install the required SSL certificate support for Python on your machine.

If you want to stick with the version of Python 3 you have already installed, please open a Terminal and run the following commands:

	cd /Applications/Python\ 3.7/
	./Install\ Certificates.command

Please replace `3.7` with the appropriate Python version if necessary.

Certificate support should work correctly after running this command.

Users from certain regions have also reported that the requested download addresses, specifically subdomains of gradle.org, might be blocked by certain ISPs. Please use a VPN if you think that you may be affected by such blocking measures.

If all else fails, please use Python 2 to run the `boden.py` tool. This should work around the SSL certificate issue with Python 3.

## Didn't find what you're looking for?

Please [let us know](../../../feedback). We're happy to help!
