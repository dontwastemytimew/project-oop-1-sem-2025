# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\DatingAgencyLogic_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\DatingAgencyLogic_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\dating_agency_tests_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\dating_agency_tests_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\project_oop_1_sem_2025_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\project_oop_1_sem_2025_autogen.dir\\ParseCache.txt"
  "DatingAgencyLogic_autogen"
  "_deps\\googletest-build\\googlemock\\CMakeFiles\\gmock_autogen.dir\\AutogenUsed.txt"
  "_deps\\googletest-build\\googlemock\\CMakeFiles\\gmock_autogen.dir\\ParseCache.txt"
  "_deps\\googletest-build\\googlemock\\CMakeFiles\\gmock_main_autogen.dir\\AutogenUsed.txt"
  "_deps\\googletest-build\\googlemock\\CMakeFiles\\gmock_main_autogen.dir\\ParseCache.txt"
  "_deps\\googletest-build\\googlemock\\gmock_autogen"
  "_deps\\googletest-build\\googlemock\\gmock_main_autogen"
  "_deps\\googletest-build\\googletest\\CMakeFiles\\gtest_autogen.dir\\AutogenUsed.txt"
  "_deps\\googletest-build\\googletest\\CMakeFiles\\gtest_autogen.dir\\ParseCache.txt"
  "_deps\\googletest-build\\googletest\\CMakeFiles\\gtest_main_autogen.dir\\AutogenUsed.txt"
  "_deps\\googletest-build\\googletest\\CMakeFiles\\gtest_main_autogen.dir\\ParseCache.txt"
  "_deps\\googletest-build\\googletest\\gtest_autogen"
  "_deps\\googletest-build\\googletest\\gtest_main_autogen"
  "dating_agency_tests_autogen"
  "project_oop_1_sem_2025_autogen"
  )
endif()
