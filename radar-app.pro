QT += quick positioning sql
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
        all_countries.cpp \
        app.cpp \
        database.cpp \
        eventsmodel.cpp \
        locationprovider.cpp \
        main.cpp

RESOURCES += qml.qrc

!android: RESOURCES += qml_desktop.qrc

RESOURCES += images/qrcode-apk.png
RESOURCES += icons/radar/index.theme \
             icons/radar/20x20/back.png \
             icons/radar/20x20/drawer.png \
             icons/radar/20x20/menu.png \
             icons/radar/20x20@2/back.png \
             icons/radar/20x20@2/drawer.png \
             icons/radar/20x20@2/menu.png \
             icons/radar/20x20@3/back.png \
             icons/radar/20x20@3/drawer.png \
             icons/radar/20x20@3/menu.png \
             icons/radar/20x20@4/back.png \
             icons/radar/20x20@4/drawer.png \
             icons/radar/20x20@4/menu.png

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

android {
    QT += androidextras
}

HEADERS += \
    all_countries.h \
    app.h \
    database.h \
    eventsmodel.h \
    locationprovider.h

android {
    ANDROID_ABIS = "armeabi-v7a"
    DISTFILES += \
                 android/AndroidManifest.xml \
                 android/build.gradle \
                 android/gradle/wrapper/gradle-wrapper.jar \
                 android/gradle/wrapper/gradle-wrapper.properties \
                 android/gradlew \
                 android/gradlew.bat \
                 android/res/values/libs.xml

    ANDROID_LIB_DEPENDENCIES = \
    libQt5AndroidExtras.so \
    libQt5Concurrent.so \
    libQt5Core.so \
    libQt5Gui.so \
    libQt5Location.so \
    libQt5Network.so \
    libQt5Positioning.so \
    libQt5PositioningQuick.so \
    libQt5Qml.so \
    libQt5Quick.so \
    libQt5QuickControls2.so \
    libQt5QuickParticles.so \
    libQt5QuickTemplates2.so \
    libQt5Sql.so \
    plugins/libplugins_bearer_libqandroidbearer.so \
    plugins/libplugins_imageformats_libqgif.so \
    plugins/libplugins_imageformats_libqicns.so \
    plugins/libplugins_imageformats_libqico.so \
    plugins/libplugins_imageformats_libqjpeg.so \
    plugins/libplugins_imageformats_libqtga.so \
    plugins/libplugins_imageformats_libqtiff.so \
    plugins/libplugins_imageformats_libqwbmp.so \
    plugins/libplugins_imageformats_libqwebp.so \
    plugins/libplugins_platforms_android_libqtforandroid.so \
    plugins/libplugins_position_libqtposition_android.so \
    plugins/libplugins_position_libqtposition_positionpoll.so \
    plugins/libplugins_sqldrivers_libqsqlite.so \
    plugins/libplugins_styles_libqandroidstyle.so \
    libqml_QtGraphicalEffects_libqtgraphicaleffectsplugin.so \
    libqml_QtGraphicalEffects_private_libqtgraphicaleffectsprivate.so \
    libqml_QtLocation_libdeclarative_location.so \
    libqml_QtPositioning_libdeclarative_positioning.so \
    libqml_QtQuick.2_libqtquick2plugin.so \
    libqml_QtQuick_Controls.2_Material_libqtquickcontrols2materialstyleplugin.so \
    libqml_QtQuick_Controls.2_libqtquickcontrols2plugin.so \
    libqml_QtQuick_Layouts_libqquicklayoutsplugin.so \
    libqml_QtQuick_Templates.2_libqtquicktemplates2plugin.so \
    libqml_QtQuick_Window.2_libwindowplugin.so \
    libqml_Qt_labs_platform_libqtlabsplatformplugin.so \
    libqml_Qt_labs_settings_libqmlsettingsplugin.so
}

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}
android: include($$PWD/../android_openssl/openssl.pri)
