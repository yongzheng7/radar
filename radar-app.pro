QT += quick quickcontrols2 positioning sql
QT -= widgets
QT -= gui

CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        src/all_countries.cpp \
        src/app.cpp \
        src/database.cpp \
        src/eventsmodel.cpp \
        src/locationprovider.cpp \
        src/main.cpp

RESOURCES += resources.qrc \
    translations.qrc

!android: RESOURCES += qml_desktop.qrc

RESOURCES += images/qrcode-apk.png
RESOURCES += icons/radar/index.theme \
             icons/radar/20x20/back.png \
             icons/radar/20x20/drawer.png \
             icons/radar/20x20/empty.png \
             icons/radar/20x20/menu.png \
             icons/radar/20x20@2/back.png \
             icons/radar/20x20@2/drawer.png \
             icons/radar/20x20@2/empty.png \
             icons/radar/20x20@2/menu.png \
             icons/radar/20x20@3/back.png \
             icons/radar/20x20@3/drawer.png \
             icons/radar/20x20@3/menu.png \
             icons/radar/20x20@4/back.png \
             icons/radar/20x20@4/drawer.png \
             icons/radar/20x20@4/empty.png \
             icons/radar/20x20@4/menu.png

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/all_countries.h \
    src/app.h \
    src/database.h \
    src/eventsmodel.h \
    src/locationprovider.h

TRANSLATIONS += \
    i18n/radar_ru.ts \
    i18n/radar_de.ts \
    i18n/radar_en.ts


android {
    QT += androidextras
    QT -= location
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
    ANDROID_ABIS = "armeabi-v7a"
    DISTFILES += \
                 android/AndroidManifest.xml \
                 android/build.gradle \
                 android/gradle/wrapper/gradle-wrapper.jar \
                 android/gradle/wrapper/gradle-wrapper.properties \
                 android/gradlew \
                 android/gradlew.bat \
                 android/res/values/libs.xml \
                 android/res/values/styles.xml 

    include($$PWD/android_openssl/openssl.pri)
}

