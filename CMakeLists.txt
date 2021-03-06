cmake_minimum_required(VERSION 3.7...3.18)

if(${CMAKE_VERSION} VERSION_LESS 3.18)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.18)
endif()

cmake_policy(SET CMP0048 NEW)
project(RadarApp
    VERSION 0.1
        DESCRIPTION "Radar Squat Net Mobile App"
        LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(DOWNLOAD_URL ${APK_DOWNLOAD_URL} CACHE STRING "Apk download URL")

set(IMAGE_RESOURCES
    images/qrcode-apk.png
)

set(QRC_SOURCES ${IMAGE_RESOURCES})
set(CMAKE_GENERATED_RCC ${CMAKE_BINARY_DIR}/cmake_generated.qrc)
file(WRITE ${CMAKE_GENERATED_RCC}
"<!DOCTYPE RCC><RCC version=\"1.0\">\n"
"<qresource prefix=\"/\">\n"
)

# Android?
foreach(RES_FILE ${QRC_SOURCES})
    get_filename_component(FULL_PATH ${RES_FILE} ABSOLUTE)
    file(APPEND ${CMAKE_GENERATED_RCC}
        "<file alias=\""
        ${RES_FILE}
        "\">"
        ${FULL_PATH}
        "</file>\n"
        )
endforeach(RES_FILE)
file(APPEND ${CMAKE_GENERATED_RCC}
"</qresource>\n"
"</RCC>\n"
)

set(FONTS_RCC "${CMAKE_BINARY_DIR}/fonts.qrc")
set(SOURCE_FONT "${CMAKE_CURRENT_LIST_DIR}/fonts/materialdesignicons-webfont.ttf")
set(GLYPH_LIST "${CMAKE_CURRENT_LIST_DIR}/fonts/used_glyphs.txt")
configure_file(fonts/fonts.qrc "${FONTS_RCC}")

add_custom_target(ICON_FONT pyftsubset --output-file="${CMAKE_BINARY_DIR}/icons.ttf" --unicodes-file="${GLYPH_LIST}" "${SOURCE_FONT}"
    BYPRODUCTS "${CMAKE_BINARY_DIR}/icons.ttf")


set(CXX_SOURCES
    src/all_countries.cpp
    src/app.cpp
    src/database.cpp
    src/eventsmodel.cpp
    src/locationprovider.cpp
    src/main.cpp
    src/all_countries.h
    src/app.h
    src/database.h
    src/eventsmodel.h
    src/locationprovider.h)


if(ANDROID)
    find_package(Qt5 COMPONENTS Core AndroidExtras Quick QuickControls2 Positioning Sql REQUIRED)

    if(ANDROID_SSL_LIBS)
        set(ANDROID_EXTRA_LIBS ${ANDROID_EXTRA_LIBS} ${ANDROID_SSL_LIBS})
    else()
        include(android_openssl/CMakeLists.txt)
    endif()

    include(qt-android-cmake/AddQtAndroidApk.cmake)

    add_library(radar-app SHARED
        ${CXX_SOURCES}
        translations.qrc
        resources.qrc
        qml_desktop.qrc
        ${CMAKE_GENERATED_RCC}
        ${FONTS_RCC}
    )

    target_compile_definitions(radar-app PRIVATE APK_DOWNLOAD_URL="${DOWNLOAD_URL}")

    qt5_use_modules(radar-app Core AndroidExtras Quick QuickControls2 Positioning Sql)

    add_qt_android_apk(radar-app_apk radar-app
        NAME "Radar App"
        VERSION_CODE 1
        PACKAGE_NAME "org.radar.app"
        PACKAGE_SOURCES "${CMAKE_CURRENT_LIST_DIR}/android"
        DEPENDS "${ANDROID_EXTRA_LIBS}"
        )
else()
    find_package(Qt5 COMPONENTS Core Quick Positioning QuickControls2 Sql REQUIRED)
    add_executable(radar-app
        ${CXX_SOURCES}
        translations.qrc
        resources.qrc
        qml_desktop.qrc
        ${CMAKE_GENERATED_RCC}
        ${FONTS_RCC}
    )
    qt5_use_modules(radar-app Core Quick QuickControls2 Positioning Sql)
endif()

add_dependencies(radar-app ICON_FONT)
set_target_properties(radar-app
                      PROPERTIES CXX_STANDARD_REQUIRED 14)

