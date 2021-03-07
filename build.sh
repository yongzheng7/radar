#!/bin/bash
script ../radar-app/properties.local
~/qt/5.12.9/android_armv7/bin/qmake CONFIG+=release ../radar-app 
make
make install INSTALL_ROOT=.
~/qt/5.12.9/android_armv7/bin/androiddeployqt --input android-libradar-app.so-deployment-settings.json --output . --gradle --no-gdbserver
