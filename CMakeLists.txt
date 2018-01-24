cmake_minimum_required(VERSION 2.8.4)

PROJECT(JNIDlib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "lib/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")

find_package(Java REQUIRED)
find_package(JNI REQUIRED)
include(UseJava)

set(CMAKE_JAVA_COMPILE_FLAGS "-source" "1.6" "-target" "1.6")
add_jar(JNIDlib JNIDlib.java)
get_target_property(_jarFile JNIDlib JAR_FILE)
get_target_property(_classDir JNIDlib CLASSDIR)

# generate JNIDlib.h stub
set (_stubDir "${CMAKE_CURRENT_BINARY_DIR}")
add_custom_command(
    OUTPUT JNIDlib.h
    COMMAND ${Java_JAVAH_EXECUTABLE} -verbose
        -classpath ${_classDir}
        -d ${_stubDir}
        -jni com.bakaoh.JNIDlib
    DEPENDS JNIDlib
    )

# generate libdlib-jni.so
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
include(./lib/dlib/dlib/cmake)
include_directories(${JNI_INCLUDE_DIRS} ${_classDir} ${_stubDir})

find_package(OpenCV)
if (OpenCV_FOUND)
    include_directories(${OpenCV_INCLUDE_DIRS})

    add_library(dlibjni MODULE JNIDlib.cpp JNIDlib.h)
    target_link_libraries(dlibjni dlib ${OpenCV_LIBS} )
else ()
    add_library(dlibjni MODULE JNIDlib.cpp JNIDlib.h)
    target_link_libraries(dlibjni dlib)
endif ()
