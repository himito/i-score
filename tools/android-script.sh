#!/bin/bash -x
BASE_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/..

export ANDROID_NATIVE_API_LEVEL=19

export QT_SDK=/home/jcelerier/Qt/5.6
export ANDROID_NDK=/opt/android-ndk
export ANDROID_SDK=/opt/android-sdk
export PATH=$ANDROID_SDK/tools:$ANDROID_SDK/platform-tools:$ANDROID_SDK/build-tools/android-4.4W:$PATH
export JAVA_HOME=/usr/lib/jvm/default
export ANT=/usr/bin/ant

cmake -DCMAKE_TOOLCHAIN_FILE=$BASE_DIR/CMake/Android/android.toolchain.cmake -DCMAKE_PREFIX_PATH=$QT_SDK/android_armv7/lib/cmake/Qt5 $BASE_DIR
make -j8
make apk_debug

adb uninstall net.iscore
adb install base/app-android/bin/iscore-Android-debug-armeabi-v7a-03.apk
adb shell am start -n net.iscore/org.qtproject.qt5.android.bindings.QtActivity
