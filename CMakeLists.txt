cmake_minimum_required(VERSION 3.30)
project(Babble)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "D:/Qt/6.8.2/mingw_64/lib/cmake")

find_package(Qt6 COMPONENTS
    Core
    Gui
    Widgets
    REQUIRED)

find_package(CURL REQUIRED)
#find_package(OpenCV CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)

#message all opencv info
message(STATUS "OpenCV library status:" )
message(STATUS "    version: ${OpenCV_VERSION}" )
message(STATUS "    libraries: ${OpenCV_LIBS}" )
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}" )


#add_executable(Babble main.cpp
#    main_window.ui
#    main_window.cpp
#    include/camera.hpp
#    camera.cpp
#    include/serial.hpp
#    serial.cpp
##        include/inference.hpp
##        inference.cpp
#)
#
#target_include_directories(
#    Babble PUBLIC
#    include
#)
#
#
#target_link_libraries(Babble
#    Qt::Core
#    Qt::Gui
#    Qt::Widgets
#    CURL::libcurl
#    ${OpenCV_LIBS}
#    Eigen3::Eigen
#    #add windows library
#    Setupapi
#    User32
#)

add_executable(VideoInferTest
        inference.cpp
        inference_test.cpp
)

target_include_directories(
        VideoInferTest PUBLIC
        include
        3rdparty/include/opencv4
)

target_link_directories(
        VideoInferTest PUBLIC
        3rdparty/lib
)

target_link_libraries(
        VideoInferTest PUBLIC
        Eigen3::Eigen
        opencv_core4
        opencv_imgproc4
        opencv_highgui4
        opencv_imgcodecs4
        opencv_videoio4
        opencv_video4
        opencv_dnn4
        opencv_calib3d4
        opencv_features2d4
        opencv_flann4
        opencv_objdetect4
        opencv_ml4
        opencv_photo4
        opencv_stitching4
)


if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if(NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if(NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif()
    endif()
    if(EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif()
    foreach(QT_LIB Core Gui Widgets )
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach(QT_LIB)
endif()
