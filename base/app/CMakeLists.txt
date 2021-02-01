cmake_minimum_required(VERSION 3.0)
project(score_app LANGUAGES CXX)

score_common_setup()
score_write_static_plugins_header()
enable_testing()
set(APPNAME "segment-editor")
set(CMAKE_POSITION_INDEPENDENT_CODE 1)

add_executable(${APPNAME} WIN32
  "${CMAKE_CURRENT_SOURCE_DIR}/Application.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Application.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

target_link_libraries(${APPNAME} PUBLIC score_lib_base)
if(SCORE_STATIC_PLUGINS)
  target_link_libraries(${APPNAME} PUBLIC ${SCORE_PLUGINS_LIST})
endif()

if(MSVC)
  target_link_libraries(${APPNAME} PRIVATE)
  target_sources(${APPNAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../lib/resources/segment.rc")
endif()

set_target_properties(${APPNAME}
  PROPERTIES
    OUTPUT_NAME segment-editor)

if(UNIX AND NOT APPLE)
  target_link_libraries(${APPNAME} PUBLIC X11)
endif()

if(APPLE)
    target_sources(${APPNAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/mac_main.m")

    set_source_files_properties(
      "${CMAKE_CURRENT_SOURCE_DIR}/mac_main.m"
      PROPERTIES
        SKIP_UNITY_BUILD_INCLUSION 1
    )

    find_library(CFLIB CoreFoundation)
    find_library(FLIB Foundation)
    target_link_libraries(${APPNAME} PRIVATE ${CFLIB} ${FLIB})

    if(DEPLOYMENT_BUILD)
      set(ICON_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../lib/resources/segment.icns")
      set_source_files_properties(${ICON_SRC} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

      target_sources(${APPNAME} PRIVATE ${ICON_SRC})
      set_target_properties(${APPNAME} PROPERTIES MACOSX_BUNDLE TRUE)
    endif()
endif()

setup_score_common_exe_features(${APPNAME})

if(UNIX AND NOT APPLE)
install(
  TARGETS ${APPNAME}
  RUNTIME DESTINATION bin)
else()
install(
  TARGETS ${APPNAME}
  BUNDLE DESTINATION .
  RUNTIME DESTINATION .)
endif()

include(IScoreDeployment)
