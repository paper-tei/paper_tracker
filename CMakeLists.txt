cmake_minimum_required(VERSION 3.30)
project(Babble)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# if toolchain is mingw
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "D:/QtMVSC/6.8.2/msvc2022_64")
set(OpenCV_DIR "D:/Babble3rdParty/opencv-mvsc/opencv/build")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MT")
set(ONNXRUNTIME_ROOT "D:/Babble3rdParty/onnxruntime-win-x64-1.20.1/onnxruntime-win-x64-1.20.1")

find_package(OpenCV CONFIG REQUIRED)

find_package(Qt6 COMPONENTS
    Core
    Gui
    Widgets
    REQUIRED)

add_executable(Babble main.cpp
    main_window.ui
    main_window.cpp
    camera.cpp
    serial.cpp
    video_reader.cpp
    inference.cpp
)

target_include_directories(
    Babble PUBLIC
    include
    ${OpenCV_INCLUDE_DIRS}
    ${ONNXRUNTIME_ROOT}/include
)

target_link_directories(
    Babble PUBLIC
    ${ONNXRUNTIME_ROOT}/lib
)

target_link_libraries(Babble PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    ${OpenCV_LIBS}
    onnxruntime
    #add windows library
    Setupapi
    User32
)

# install dir model to the same dir as the executable
install(DIRECTORY ${CMAKE_SOURCE_DIR}/model/  DESTINATION ${CMAKE_BINARY_DIR}/model)


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
