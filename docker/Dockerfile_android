FROM ubuntu:18.04

ARG ANDROID_BUILD_API_VERSION=28
ARG ANDROID_BUILD_TOOLS_VERSION=28.0.3
ARG ANDROID_ABI=x86_64
ARG GRADLE_VERSION=4.10.2

ARG user=jenkins
ARG group=jenkins

RUN dpkg --add-architecture i386

RUN apt-get update \
 && apt-get install -y \
    curl \
    unzip \
    xvfb \
    xauth \
    python \
    librsvg2-bin \
    pciutils \
    openjdk-8-jre \
    openjdk-8-jdk \
    x11-apps \
    libc6:i386 \
    libncurses5:i386 \
    libstdc++6:i386 \
    lib32z1 \
    libbz2-1.0:i386 \
    cmake \
    ninja-build \
    git


RUN curl --url "https://dl.google.com/android/repository/sdk-tools-linux-3859397.zip" --output /tmp/androidtools.zip

RUN mkdir /opt/android
RUN chmod -R go+rwx /opt/android
RUN unzip -d /opt/android /tmp/androidtools.zip
RUN rm /tmp/androidtools.zip

ENV ANDROID_HOME=/opt/android

# the android tools try to write user data to the home directory.
# If we do not configure one then the home directory will be /, where normal users
# cannot create anything. Note that we run as a non-root user when the docker
# file is used from a Jenkins CI Server.
# So, to ensure that all that works, we create a custom home directory to which
# everyone has access.
RUN mkdir /userhome

# allow everyone access, to our virtual home directory
RUN chmod -R go+rwx /userhome
ENV HOME=/userhome

# accept all licenses from the Android SDK and needed tools.
# If we do not do this here then sdkmanager will interactively prompt
# and ask for us to confirm.

RUN yes | /opt/android/tools/bin/sdkmanager --licenses

# install the needed SDK tools and packages
RUN yes | /opt/android/tools/bin/sdkmanager \
	"platform-tools" \
	"ndk-bundle" \
	"extras;android;m2repository" \
	"extras;google;m2repository" \
	"build-tools;$ANDROID_BUILD_TOOLS_VERSION" \
	"platforms;android-$ANDROID_BUILD_API_VERSION"









