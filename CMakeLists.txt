# CMakeList.txt : CMake project for BreathLibraryCore, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("BreathLibraryCore")

message(STATUS "Project: ${PROJECT_NAME}")

# Add source to this project's executable.
add_compile_definitions(DLL_EXPORTS)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Create variable for all source files (all .cpp and .hpp files in src folder)
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.hpp")

set(TF_ROOT_DIR C:/Users/nevin/Documents/UnityProjects_2022/TheBreathLibrary/tflite-dist)

include_directories(${TF_ROOT_DIR}/include)

if (WIN32)
  set(TENSORFLOW_LOCATION ${TF_ROOT_DIR}/libs/windows_x86_64)
  find_library(TENSORFLOW_LIBRARY tensorflowlite_c.dll.if ${TENSORFLOW_LOCATION})
  link_libraries(${TENSORFLOW_LIBRARY})

  message(STATUS "Tensorflow library: ${TENSORFLOW_LIBRARY}")

elseif (ANDROID)
  add_library(tensorflowlite_c SHARED IMPORTED)
  set_target_properties(tensorflowlite_c PROPERTIES IMPORTED_LOCATION ${TF_ROOT_DIR}/libs/android/${ANDROID_ABI}/libtensorflowlite_c.so)
  
  message(STATUS "Android ABI: ${ANDROID_ABI}")
endif()

add_library (BreathLibraryCore SHARED ${SOURCE_FILES})


# if (ANDROID)
#   target_link_libraries(BreathLibraryCore tensorflowlite_c)
# endif()


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET BreathLibraryCore PROPERTY CXX_STANDARD 20)
endif()