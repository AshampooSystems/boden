# Boden

*Image representing core caps of Boden. Make it easy for people to star the repo even if they don't have time to read.*

*Teaser: What is Boden in two to three short sentences.*

## Getting Started

Android apps can be built on macOS, Windows, and Linux. iOS apps can be built on macOS only.

Choose your development platform:

* [macOS](#developing-boden-apps-on-macos)
* Windows
* Linux

### Developing Boden Apps on macOS

Choose your target platform: [iOS](#ios-apps) or [Android](#android-apps).

### iOS Apps

Boden requires the following dependencies to be installed:

1. macOS 10.13.6 High Sierra or higher
2. Xcode 9.2 or higher (with Command Line Tools installed)
3. Python 3.4 or higher
4. CMake 3.12 or higher

Follow these steps to set up Boden for iOS development:

1. If you are not sure whether all of the dependencies listed above are installed, jump to the [Extended Installation Guide for iOS on macOS](https://github.com/AshampooSystems/boden/tree/feature/BDN-285-getting-started#set-up-macos-for-ios-builds) and follow the steps listed there.
2. Open the Terminal app and clone the git repository by typing `git clone git@github.com:AshampooSystems/boden.git`.
3. Type `cd boden`.
4. Type `./boden prepare -p ios`. This will create an Xcode project for you.
5. To open the generated Xcode project, type `open build/ios/std/Xcode/boden.xcodeproj`.
6. Select the `uidemo` target and press Cmd+R to build and run the example application.

### Android Apps

Boden requires the following dependencies to be installed:

1. macOS 10.13.6 High Sierra or higher
2. [Android Studio](https://developer.android.com)
3. Command Line Tools
4. OpenJDK 8
5. Python 3.4 or higher
6. CMake 3.12 or higher

Follow these steps to set up Boden for Android development on the Mac:

1. If you are not sure whether all of the dependencies listed above are installed, jump to the [Extended Installation Guide for Android on macOS](#set-up-macos-for-android-builds) and follow the steps listed there.
2. Open the Terminal app and clone the git repository by typing `git clone git@github.com:AshampooSystems/boden.git`.
3. Type `cd boden`.
4. Type `./boden prepare -p android`. This will generate an AndroidStudio project for you.
5. Run Android Studio and open `boden/build/android/std/AndroidStudio`.
6. Select the `uidemo` target and click the Run button in the toolbar to build and run the example application.

## Extended Installation Guide

### Set up macOS for iOS builds

* Install brew: `/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"`
* `brew install python3 cmake`
* Install Xcode
    * Open App Store app by clicking on the Apple logo in the menu bar and selecting “App Store..."
    * Click on the search field (macOS < 10.14 upper right corner of the window, macOS 10.14 upper left corner of the window) and type “Xcode"
    * Locate “Xcode” and click on “Get”
    * The store page for Xcode opens up
    * Click the “Get” button again
    * Once installed, open Xcode and agree to the license agreement
    * Xcode will install components afterwards
    * Open a terminal and run sudo xcodeselect -s /Applications/Xcode.app/Contents/Developer
* `git clone git@github.com:AshampooSystems/boden.git`
* If git not installed:
    * macOS asks if software should be downloaded
    * Accept EULA
    * Download
    * “Software was installed"
* `cd boden`
* `./boden build -p ios -b Xcode -c Debug -t uidemo`

### Set up macOS for Android builds

* Install brew: `/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"`
* `brew tap caskroom/versions`
* `brew cask install java8`
* `brew install python3 cmake`
* Install Android Studio:
    * Go to https://developer.android.com
    * Click “Download Android Studio"
    * Agree to the Terms and Conditions
    * Once the download is completed, open the DMG
    * Drag Android Studio.app and drop it on the Applications folder icon
    * Open Android Studio
    * In the dialog asking you to complete the installation, click ok
    * Android Studio Setup Wizard opens up
    * Click Next
    * Leave type of setup set to “Standard” and click Next
    * Choose a theme and click Next
    * Leave the SDK components setup as is and click Next
    * Click Finish
    * Android Studio will now download a couple of components, grab a snack and a drink
    * On the summary screen, click Finish
    * On the “Welcome to Android” screen, click “Configure” and select “SDK Manager"
    * Click on the “SDK Tools” tab
    * Select “NDK” from the list
    * Click OK
    * Confirm the change by clicking OK in the popup window
    * Accept the NDK license agreement
    * The NDK component is being installed, grab another drink
    * After the installation has finished, click Finish.
* `cd boden`
* `./boden build -p android -b AndroidStudio -t uidemo -c Debug`

