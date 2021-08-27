# Radar App

Application for getting local alternative and radical events using [radar.squat.net](https://radar.squat.net) site API.

**Platforms:** Desktop and Android are supported at the moment.

Main platform is Android, Desktop is used mostly for development purpose.
Thus, UI of the desktop version may look inconsistent and unpolished.

Other platforms supported by Qt may require code adaptation.

**Ports are welcome!**

**Programming Languages used:** C++, JavaScript
*Framework:* Qt5, QML

## Disclaimer

Application author is not responsible for any contents published at https://radar.squat.net

## Building for Android

Needed tools:

* Qt 5.12 for android_armv7 arch [download.qt.io/official_releases/qt/5.12/](https://download.qt.io/official_releases/qt/5.12/)
* CMake 3.10+ https://cmake.org/download/
* Android NDK 19+ https://developer.android.com/studio
* Android SDK 21+ https://developer.android.com/studio
* GNU Make https://www.gnu.org/software/make/
* FontTools https://github.com/fonttools/fonttools

### Building

1. Create a build directory, e.g. build-android *outside* of source directory. Switch to it.
```
mkdir ../build-android
cd ../build-android
```
1. Set environment variables to match your installation:
```
export ANDROID_NDK=/opt/Android/Sdk/ndk/19.2.5345600
export ANDROID_SDK=/opt/Android/Sdk
export QT_BASE=/opt/qt/5.12.9/android_armv7
```
1. Run CMake to configure build:
```
cmake -DQT_ANDROID_BUILD_TYPE="--debug" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE="${ANDROID_NDK}/build/cmake/android.toolchain.cmake" -DANDROID_ABI=armeabi-v7a -DCMAKE_PREFIX_PATH:PATH="${QT_BASE}/lib" -DCMAKE_FIND_ROOT_PATH="${QT_BASE}/" -DDOWNLOAD_URL="https://my-domain.com/downloads/RadarApp.apk" ../radar-app
```
1. Build project
```
make
```

The resulting APK could be found at `radar-app-armeabi-v7a//build/outputs/apk/debug/radar-app-armeabi-v7a-debug.apk`

## CI Build

GitLab CI build recipe is described here: `.gitlab-ci.yml`
(Files starting with '.' are hidden in UNIX-like OS'es)

The recipe takes care on downloading and installing all the dependencies.

**F-Droid metadata and CI recipe will be added soon!**

## Third Party Dependencies used

### Android OpenSSL support for Qt
Used to provide OpenSSL libraries libssl and libcrypto.

**Origin:** https://github.com/KDAB/android_openssl.git

**License:** [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0)

### Qt Android CMake utility script
Used to perform APK build using CMake

**Origin:** https://github.com/LaurentGomila/qt-android-cmake

**License:** [3-Clause BSD](https://opensource.org/licenses/BSD-3-Clause)

### Qt Downloader
Used to automate Qt download during CI builds

**Origin:** https://github.com/engnr/qt-downloader

**License:** [MIT License](https://github.com/engnr/qt-downloader/blob/master/LICENSE)

### Material Design Icons WebFont
Used as Icon Font in the app.

Original font gets minified with pyftsubset tool (See `fonts/create_icon_font.sh`).
Only glyphs listed in `fonts/used_glyphs.txt` are included

**Origin:** https://github.com/Templarian/MaterialDesign-Webfont/

**License:** [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0)

## Contributing

Main source repository is located here: https://0xacab.org/xandyx/radar-app/

Feel free to provide pull requests and issue reports.

For reporing Issues, use this link: https://0xacab.org/xandyx/radar-app/-/issues

