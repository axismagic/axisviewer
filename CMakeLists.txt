cmake_minimum_required(VERSION 2.6.2)
project(axisviewer)

INCLUDE(FindPkgConfig)

#hack - add -std=c++0x to CXXFLAGS
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

set(UI_SOURCES ui/image_transport.cpp ui/main_window.cpp)
set(MAIN_SOURCES camera_viewer.cpp image_processing.cpp stream_handler.cpp main.cpp)

set(SOURCES ${UI_SOURCES} ${MAIN_SOURCES})

add_executable(axisviewer ${SOURCES})

#dependencies
if(${PKG_CONFIG_FOUND})
	#use pkg-config if available
	pkg_check_modules(CURL REQUIRED libcurl)
	pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)
	include_directories(${GTKMM_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS})
	target_link_libraries(axisviewer ${GTKMM_LIBRARIES} ${CURL_LIBRARIES})
else()
	find_package(GTK3 REQUIRED gtkmm)
	find_package(CURL REQUIRED)
	include_directories(${GTK3_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS})
	target_link_libraries(axisviewer ${GTK3_LIBRARIES} ${CURL_LIBRARY})
endif()

