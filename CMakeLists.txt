cmake_minimum_required(VERSION 3.7...3.18)

if(${CMAKE_VERSION} VERSION_LESS 3.18)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.18)
endif()

project(RadarApp VERSION 1.0
        DESCRIPTION "Radar Squat Net Mobile App"
        LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(IMAGE_RESOURCES images/qrcode-apk.png)
set(ICON_RESOURCES
    icons/radar/index.theme
    icons/radar/20x20/back.png
    icons/radar/20x20/drawer.png
    icons/radar/20x20/menu.png
    icons/radar/20x20@2/back.png
    icons/radar/20x20@2/drawer.png
    icons/radar/20x20@2/menu.png
    icons/radar/20x20@3/back.png
    icons/radar/20x20@3/drawer.png
    icons/radar/20x20@3/menu.png
    icons/radar/20x20@4/back.png
    icons/radar/20x20@4/drawer.png
    icons/radar/20x20@4/menu.png
)

set(QRC_SOURCES ${IMAGE_RESOURCES} ${ICON_RESOURCES})
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
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)

    add_library(radar-app
        ${CXX_SOURCES}
        resources.qrc
        qml_desktop.qrc
        ${CMAKE_GENERATED_RCC}
    )
    include(android_openssl/CMakeLists.txt)
    include(qt-android-cmake/AddQtAndroidApk.cmake)
    add_qt_android_apk(radar-app_apk radar-app)
else()
    add_executable(radar-app
        ${CXX_SOURCES}
        resources.qrc
        qml_desktop.qrc
        ${CMAKE_GENERATED_RCC}
    )
endif()

find_package(Qt5 COMPONENTS Quick Positioning Sql REQUIRED)

set_target_properties(radar-app 
                      PROPERTIES CXX_STANDARD_REQUIRED 14)

qt5_use_modules(radar-app Quick Positioning Sql)
