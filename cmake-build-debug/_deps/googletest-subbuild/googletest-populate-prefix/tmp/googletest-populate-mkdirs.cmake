# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/clionprojects/project-oop-1-sem-2025/cmake-build-debug/_deps/googletest-src"
  "D:/clionprojects/project-oop-1-sem-2025/cmake-build-debug/_deps/googletest-build"
  "D:/clionprojects/project-oop-1-sem-2025/cmake-build-debug/_deps/googletest-subbuild/googletest-populate-prefix"
  "D:/clionprojects/project-oop-1-sem-2025/cmake-build-debug/_deps/googletest-subbuild/googletest-populate-prefix/tmp"
  "D:/clionprojects/project-oop-1-sem-2025/cmake-build-debug/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp"
  "D:/clionprojects/project-oop-1-sem-2025/cmake-build-debug/_deps/googletest-subbuild/googletest-populate-prefix/src"
  "D:/clionprojects/project-oop-1-sem-2025/cmake-build-debug/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/clionprojects/project-oop-1-sem-2025/cmake-build-debug/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/clionprojects/project-oop-1-sem-2025/cmake-build-debug/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
