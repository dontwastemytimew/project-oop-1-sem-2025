cmake_minimum_required(VERSION 3.29)
project(project_oop_1_sem_2025)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Sql Gui Widgets Charts LinguistTools REQUIRED)

include_directories(models core gui utils)
include_directories(. models core gui utils)


file(GLOB TS_SOURCES
        "core/*.cpp"
        "gui/*.cpp"
        "gui/*.h"
        "models/*.cpp"
        "models/*.h"
        "utils/*.cpp"
        "utils/*.h"
        "main.cpp"
)

qt_add_translations(project_oop_1_sem_2025
        QM_FILES_OUTPUT_DIR "${CMAKE_BINARY_DIR}/i18n"
        TS_FILES
            i18n/app_ua.ts
            i18n/app_en.ts
)


add_library(DatingAgencyLogic STATIC
        models/DatabaseManager.cpp models/DatabaseManager.h
        models/UserProfile.cpp models/UserProfile.h
        models/ContactInfo.cpp models/ContactInfo.h
        utils/UserLogger.cpp utils/UserLogger.h
        models/Preference.cpp models/Preference.h
        core/MatchEngine.cpp core/MatchEngine.h
        core/FakeDataManager.cpp core/FakeDataManager.h
)

target_link_libraries(DatingAgencyLogic PUBLIC
        Qt6::Core
        Qt6::Sql
        Qt6::Widgets
        Qt6::Charts
)

add_executable(project_oop_1_sem_2025
        main.cpp
        gui/mainwindow.cpp
        gui/profilecard.cpp
        gui/searchpagewidget.cpp
        gui/profilepagewidget.cpp
        gui/matchespagewidget.cpp
        gui/settingspagewidget.cpp
        resources.qrc
        gui/welcomepagewidget.cpp
        gui/adminpagewidget.cpp
        gui/StatsDialog.cpp
)

target_link_libraries(project_oop_1_sem_2025 PRIVATE
        DatingAgencyLogic
      
        Qt6::Gui
        Qt6::Widgets
        Qt6::Charts
        Qt6::Sql
        Qt6::Core
        
        ${project_oop_1_sem_2025_TRANSLATION_FILES}
)


include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/v1.14.0.zip
)

FetchContent_MakeAvailable(googletest)
include(GoogleTest)

add_executable(dating_agency_tests
        tests/tests_main.cpp
        tests/DatabaseManagerTests.cpp
        tests/matchengine_tests.cpp
)

target_link_libraries(dating_agency_tests PRIVATE
        DatingAgencyLogic
        Qt6::Core
        Qt6::Sql
        gtest_main
)

add_executable(project_benchmark
        tests/benchmark_main.cpp
        utils/BenchmarkTool.h
)

target_link_libraries(project_benchmark PRIVATE
        DatingAgencyLogic
        Qt6::Core
        Qt6::Sql
        Qt6::Charts
)