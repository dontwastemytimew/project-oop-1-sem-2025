cmake_minimum_required(VERSION 3.29)
project(project_oop_1_sem_2025)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Sql Gui Widgets REQUIRED)

include_directories(.)

add_library(DatingAgencyLogic STATIC
        DatabaseManager.cpp DatabaseManager.h
        UserProfile.cpp UserProfile.h
        ContactInfo.cpp ContactInfo.h
        UserLogger.cpp UserLogger.h
)

target_link_libraries(DatingAgencyLogic PUBLIC
        Qt6::Core
        Qt6::Sql
)

add_executable(project_oop_1_sem_2025 main.cpp)
target_link_libraries(project_oop_1_sem_2025 PRIVATE
        DatingAgencyLogic
        Qt6::Gui
        Qt6::Widgets
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
        tests/DatabaseManagerTests.cpp
        tests/tests_main.cpp
        Preference.h
        Preference.cpp
)


target_link_libraries(dating_agency_tests PRIVATE
        DatingAgencyLogic
        gtest_main
)