#!/bin/bash

#export ANDROID_NDK=/opt/Android/Sdk/ndk/19.2.5345600
#export ANDROID_SDK=/opt/Android/Sdk
#export QT_BASE=/opt/qt/5.12.9/android_armv7

cmake -DQT_ANDROID_BUILD_TYPE="--debug" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE="${ANDROID_NDK}/build/cmake/android.toolchain.cmake" -DANDROID_ABI=armeabi-v7a -DCMAKE_PREFIX_PATH:PATH="${QT_BASE}/lib" -DCMAKE_FIND_ROOT_PATH="${QT_BASE}/" -DDOWNLOAD_URL="https://0xacab.org/xandyx/radar-app/-/jobs/artifacts/master/raw/build-android/radar-app-armeabi-v7a/build/outputs/apk/debug/radar-app-armeabi-v7a-debug.apk?job=assembleDebug" ../radar-app
