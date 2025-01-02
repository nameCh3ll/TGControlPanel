# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/name_chell/TGControlPanel/build/_deps/tgbotcpp-src")
  file(MAKE_DIRECTORY "/home/name_chell/TGControlPanel/build/_deps/tgbotcpp-src")
endif()
file(MAKE_DIRECTORY
  "/home/name_chell/TGControlPanel/build/_deps/tgbotcpp-build"
  "/home/name_chell/TGControlPanel/build/_deps/tgbotcpp-subbuild/tgbotcpp-populate-prefix"
  "/home/name_chell/TGControlPanel/build/_deps/tgbotcpp-subbuild/tgbotcpp-populate-prefix/tmp"
  "/home/name_chell/TGControlPanel/build/_deps/tgbotcpp-subbuild/tgbotcpp-populate-prefix/src/tgbotcpp-populate-stamp"
  "/home/name_chell/TGControlPanel/build/_deps/tgbotcpp-subbuild/tgbotcpp-populate-prefix/src"
  "/home/name_chell/TGControlPanel/build/_deps/tgbotcpp-subbuild/tgbotcpp-populate-prefix/src/tgbotcpp-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/name_chell/TGControlPanel/build/_deps/tgbotcpp-subbuild/tgbotcpp-populate-prefix/src/tgbotcpp-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/name_chell/TGControlPanel/build/_deps/tgbotcpp-subbuild/tgbotcpp-populate-prefix/src/tgbotcpp-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
