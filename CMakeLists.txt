cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0115 NEW) # explicit files extensions
project(OMGMounter
    VERSION 0.1.0
    LANGUAGES CXX
)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(PROJECT_TRANSLATIONS_UPDATE                  "Update source translations              [default: OFF]" OFF)
option(PROJECT_USE_APPSTREAM_ID                     "Whether to use AppStream naming         [default: OFF]" OFF)
set(PROJECT_TRANSLATION_TEST_ENABLED 0 CACHE STRING "Whether to enable translation testing   [default: 0]")
set(PROJECT_TRANSLATION_TEST "it" CACHE STRING      "Country code of language to test in IDE [default: it]")

set(PROJECT_QT_VERSION 5 CACHE STRING "Qt version to use [default: 5]")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(QT NAMES Qt${PROJECT_QT_VERSION})
find_package(Qt${QT_VERSION_MAJOR} CONFIG REQUIRED DBus LinguistTools Widgets)
find_package(Qtilitools REQUIRED)

find_package(DBusMenuQtilities${QT_VERSION_MAJOR} REQUIRED)
find_package(StatusNotifierItemQt${QT_VERSION_MAJOR} REQUIRED)

set(PROJECT_SOURCES
    src/cdemu.cpp
    src/cdemu.hpp
    src/devicelistitem.cpp
    src/devicelistitem.hpp
    src/exception.cpp
    src/exception.hpp
    src/dialogabout.hpp
    src/dialogabout.cpp
    src/dialogabout.ui
    src/mainwindow.cpp
    src/mainwindow.hpp
    src/mainwindow.ui
    src/main.cpp
)
set(PROJECT_OTHER_FILES
    .github/workflows/build.yml
    .editorconfig
    .gitattributes
    .gitignore
    README.md
)
source_group("" FILES ${PROJECT_SOURCES})
source_group("" FILES ${PROJECT_OTHER_FILES})

include(Config.cmake)
include(QtAppResources)

set(PROJECT_ALL_FILES
    ${PROJECT_DESKTOP_FILES}
    ${PROJECT_RESOURCES}
    ${PROJECT_SOURCES}
    ${PROJECT_OTHER_FILES}
    ${PROJECT_QM_FILES}
    ${PROJECT_TRANSLATION_SOURCES}
)
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION ${PROJECT_ALL_FILES})
else()
    add_executable(${PROJECT_NAME} ${PROJECT_ALL_FILES})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Core
    Qt::DBus
    Qt::Widgets
    StatusNotifierItemQt5
)
set(PROJECT_DATA_DIR "${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_ID}")

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_ID}")

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Woverloaded-virtual)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    APPLICATION_DISPLAY_NAME="${PROJECT_NAME}"
    APPLICATION_NAME="${PROJECT_ID}"
    APPLICATION_VERSION="${PROJECT_VERSION}"
    ORGANIZATION_DOMAIN="${PROJECT_ORGANIZATION_URL}"
    ORGANIZATION_NAME="${PROJECT_ORGANIZATION_NAME}"
    PROJECT_DATA_DIR="${PROJECT_DATA_DIR}"
    PROJECT_DESCRIPTION="${PROJECT_DESCRIPTION}"
    PROJECT_TRANSLATION_TEST="${PROJECT_TRANSLATION_TEST}"
    PROJECT_TRANSLATION_TEST_ENABLED=${PROJECT_TRANSLATION_TEST_ENABLED}
)
install(TARGETS ${PROJECT_NAME}
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)
message(STATUS "
Project name:                     ${PROJECT_NAME}
AppStream ID:                     ${PROJECT_APPSTREAM_ID}
Version:                          ${PROJECT_VERSION}
Qt version:                       ${QT_VERSION}
Build type:                       ${CMAKE_BUILD_TYPE}
Install prefix:                   ${CMAKE_INSTALL_PREFIX}
Update translations before build: ${PROJECT_TRANSLATIONS_UPDATE}
")
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
