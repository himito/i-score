cmake_minimum_required(VERSION 3.1)
project(iscore_plugin_interpolation LANGUAGES CXX)

iscore_common_setup()

# Packages
find_package(Qt5 5.3 REQUIRED COMPONENTS Core Widgets)

# Files & main target
set(HDRS
  "${CMAKE_CURRENT_SOURCE_DIR}/Interpolation/Commands/ChangeAddress.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Interpolation/Commands/CommandFactory.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Interpolation/InterpolationColors.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Interpolation/InterpolationFactory.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Interpolation/InterpolationPresenter.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Interpolation/InterpolationProcess.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Interpolation/InterpolationView.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/iscore_plugin_interpolation.hpp"

)
set(SRCS
"${CMAKE_CURRENT_SOURCE_DIR}/Interpolation/InterpolationProcess.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Interpolation/InterpolationView.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Interpolation/InterpolationColors.cpp"

"${CMAKE_CURRENT_SOURCE_DIR}/Interpolation/Commands/ChangeAddress.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Interpolation/Commands/CommandFactory.cpp"

"${CMAKE_CURRENT_SOURCE_DIR}/iscore_plugin_interpolation.cpp"
)

add_library(${PROJECT_NAME} ${SRCS} ${HDRS})
iscore_generate_command_list_file(${PROJECT_NAME} "${HDRS}")

target_link_libraries(${PROJECT_NAME} PUBLIC
                     Qt5::Core Qt5::Widgets
                     iscore_lib_base iscore_lib_device iscore_plugin_deviceexplorer iscore_lib_process iscore_lib_state iscore_plugin_curve iscore_lib_inspector)

setup_iscore_plugin(${PROJECT_NAME})
