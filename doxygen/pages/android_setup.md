How to set up Android Studio for Boden development
==================================================


1. Download Android Studio: https://developer.android.com/studio

   Install it and complete the setup wizard. On Windows the wizard is part of the installation,
   on Mac and Linux it it shown on first start
  * Choose custom install
  * Make sure the "Android Virtual Device" option is activated.
  * Finish the setup wizard

2. The "Welcome to Android Studio" screen will open.
  * At the bottom there is a button "Configure". Click on it and choose "SDK Manager".
  * On the "SDK Platforms" tab activate these options:
    * The highest API level
    * Abdroid 8.0 (Oreo)
    * Android 4.1 (JellyBean)
  * Switch to the "SDK Tools" tab
  * Activate the following options:
    * CMake
    * LLDB
    * NDK
  * Click "Apply" and confirm the installation. Wait for it to finish.
  * Close the Preferences window

3. Open a terminal in the directory of your local Boden repository.

4. Generate the android project files with:

   python build.py prepare --platform android --build-system AndroidStudio

5. In Android Studio:

   * Either select "File" -> "Open" from the menu, or "Open an existing project" from the Welcome Screen
   * Select the "build/android" subdirectory of the Boden development directory

6. Android Studio will now start to analyze the project files ("gradle sync"). This will take a few seconds.

   You may get one or both of the following errors in the log at the bottom.

  * "Failed to find target": this happens when AndroidStudio generates a project for an Android
    API version that is not installed. Unfortunately this happens sometimes. To fix it, do the following:
    * From the menu, choose "Tools" -> "Android" -> "SDK Manager"
    * A list with the available API versions is shown. Look in the "API Level" column and find the highest
      number that is currently installed. Make a note of this number and close the SDK Manager.
    * In the tree on the left, under "Gradle scripts", open the "build.gradle" file for module "app".
    * Change the values of "compileSdkVersion" and "targetSdkVersion" to the number from the SDK manager.
    * At the top of the file there should be a banner with a message that gradle sync failed. Click "try again".
    * The problem should be fixed now

  * "Failed to find build tools revision XY": this message complains that the build tools are not at the right version.
    This is normal for a fresh AndroidStudio installation.
    * If it happens, there will also be a "Install Build Tools" link in this error message.
      Click on it and install the tools.  
    * After the installation the project will begin resynching, showing the "Gradle sync in progress" banner
      near the top. Wait for it to finish.


5. When the gradle sync is successful (no yellow banner at the top of the code editor window) then you can try to
   build Boden.

   *IMPORTANT*: AndroidStudio may not handle the build order correctly. So you should manually build the module "Boden" first,
   before building the others.

   * Select "Boden" in the tree.
   * From the "Build" menu, choose "Make Module boden". It should build without errors.
   * Then build the other modules by choosing "Make Project" from the "Build" menu.

6. You can now select the project to run in the toolbar (there is a combobox in the right part of the bar). "app" is uidemo, the others
   are the test applications. Select one of the applications and click on the green Play button (or the Bug+Play) next to the combobox to run it.
   It is recommended to simulate the "Nexus 5x".

7. Important note about debugging: the debugger does not support any way for the application to trigger a debug break.
   So when BDN_DEBUG_BREAK is called (e.g. when test assertions fail) then the debugger will normally simply continue and
   *not* stop automatically. However, the function debugBreakDummy() (defined in bdn/debug.h) will be called in these cases.
   It is recommended to set a breakpoint there, so that the debugger will stop.











