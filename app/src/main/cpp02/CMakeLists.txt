# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_subdirectory(loader)

add_subdirectory(angband)

add_subdirectory(faangband)

add_subdirectory(frogcomposband)

add_subdirectory(npp041)

add_subdirectory(npp054)

add_subdirectory(npp710)

add_subdirectory(silq)

#add_subdirectory(tome24x)
