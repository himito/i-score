cmake_minimum_required(VERSION 3.1)
project(iscore_plugin_loop LANGUAGES CXX)

iscore_common_setup()

# Packages
find_package(Qt5 5.3 REQUIRED COMPONENTS Core Widgets Qml)

# Files & main target
set(HDRS
  "${CMAKE_CURRENT_SOURCE_DIR}/iscore_plugin_loop.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Loop/Commands/LoopCommandFactory.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Loop/Commands/LoopTriggerCommands.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Loop/Commands/MoveLoopEvent.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Loop/Inspector/LoopInspectorFactory.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Loop/Inspector/LoopInspectorWidget.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Loop/Inspector/LoopTriggerCommandFactory.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Loop/LoopDisplayedElements.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Loop/LoopPresenter.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Loop/LoopProcessFactory.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Loop/LoopProcessMetadata.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Loop/LoopProcessModel.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Loop/LoopView.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Loop/LoopViewUpdater.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Loop/Palette/LoopToolPalette.hpp"
)
set(SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/Loop/LoopProcessModelSerialization.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Loop/LoopProcessModel.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Loop/LoopPresenter.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Loop/LoopView.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Loop/LoopDisplayedElements.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Loop/LoopViewUpdater.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Loop/Palette/LoopToolPalette.cpp"

"${CMAKE_CURRENT_SOURCE_DIR}/Loop/Commands/LoopCommandFactory.cpp"

"${CMAKE_CURRENT_SOURCE_DIR}/Loop/Inspector/LoopTriggerCommandFactory.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Loop/Inspector/LoopInspectorFactory.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Loop/Inspector/LoopInspectorWidget.cpp"

"${CMAKE_CURRENT_SOURCE_DIR}/iscore_plugin_loop.cpp"
)

add_library(${PROJECT_NAME} ${SRCS} ${HDRS})
iscore_generate_command_list_file(${PROJECT_NAME} "${HDRS}")
target_link_libraries(${PROJECT_NAME} PUBLIC
                     Qt5::Core Qt5::Widgets
                     iscore_lib_base iscore_lib_process iscore_plugin_scenario
                     iscore_lib_inspector
                     )

setup_iscore_plugin(${PROJECT_NAME})
