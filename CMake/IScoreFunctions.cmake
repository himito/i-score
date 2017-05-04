include(IScoreTargetSetup)
### Component plug-ins ###

# TODO parcourir les fichiers pour trouver les FactoryFamily.
# Faire un premier parcours ou elles sont enregistrées,
# les stocker dans une map, et rajouter en parallèle les objets correspondants.
# Puis en fonction de la configuration (statique, dynamique) et du choix d'objets
# qu'on veut avoir dans le binaire, soit générer plein de sous-projets à la fin,
# soit générer un gros projet qui contient tout ? attention si des plug-ins ont
# des dépendances sur d'autres plug-ins... (générer les deps automatiquement)

function(iscore_add_component Name Sources Headers Dependencies Version Uuid)
  foreach(target ${Dependencies})
    if(NOT TARGET ${target})
      return()
    endif()
  endforeach()

  set(ISCORE_GLOBAL_COMPONENTS_LIST
        ${ISCORE_GLOBAL_COMPONENTS_LIST} ${Name}
        CACHE INTERNAL "List of components")

  iscore_parse_components(${Name} ${Headers})

  if(ISCORE_MERGE_COMPONENTS)
    # We create a fake library that will be instantiated in component-wrapper
    add_library(${Name} INTERFACE)
    target_include_directories(${Name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
    target_sources(${Name} INTERFACE ${Sources} ${Headers})
    target_link_libraries(${Name} INTERFACE ${Dependencies})

    get_property(ComponentAbstractFactoryList GLOBAL
                 PROPERTY ISCORE_${Name}_AbstractFactoryList)
    get_property(ComponentFactoryList GLOBAL
                 PROPERTY ISCORE_${Name}_ConcreteFactoryList)
    get_property(ComponentFactoryFileList GLOBAL
                 PROPERTY ISCORE_${Name}_FactoryFileList)

    set_target_properties(${Name} PROPERTIES
      INTERFACE_ISCORE_COMPONENT_AbstractFactory_List ${ComponentAbstractFactoryList}
      INTERFACE_ISCORE_COMPONENT_ConcreteFactory_List ${ComponentFactoryList}
      INTERFACE_ISCORE_COMPONENT_Factory_Files ${ComponentFactoryFileList}
      INTERFACE_ISCORE_COMPONENT_Version ${Version}
      INTERFACE_ISCORE_COMPONENT_Key ${Uuid}
      )
  else()
    # concrete library
    iscore_generate_plugin_file(${Name} ${Headers} ${Version} ${Uuid})
    add_library(${Name}
        ${Sources} ${Headers}
        "${CMAKE_CURRENT_BINARY_DIR}/${Name}_plugin.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/${Name}_plugin.hpp"
        )

    target_link_libraries(${Name} PUBLIC ${Dependencies})

    setup_iscore_plugin(${Name})
  endif()
endfunction()

function(iscore_parse_components TargetName Headers)
  # Initialize our lists
  set(ComponentAbstractFactoryList)
  set(ComponentFactoryList)
  set(ComponentFactoryFileList)

  # First look for the ISCORE_CONCRETE_COMPONENT_FACTORY(...) ones
  foreach(header ${Headers})
      file(READ "${header}" fileContent)
      string(REGEX MATCHALL "ISCORE_CONCRETE_COMPONENT_FACTORY\\([A-Za-z_0-9\,\:\r\n\t ]*\\)" fileContent "${fileContent}")

      foreach(fileLine ${fileContent})
          string(REPLACE "\n" "" fileLine "${fileLine}")
          string(REPLACE "\r" "" fileLine "${fileLine}")

          string(STRIP "${fileLine}" strippedLine)

          string(REPLACE "ISCORE_CONCRETE_COMPONENT_FACTORY(" "" strippedLine ${strippedLine})
          string(REPLACE ")" "" strippedLine ${strippedLine})
          string(REPLACE "," ";" lineAsList ${strippedLine})
          list(GET lineAsList 0 AbstractClassName)
          list(GET lineAsList 1 ClassName)
          string(STRIP ${AbstractClassName} strippedAbstractClassName)
          string(STRIP ${ClassName} strippedClassName)
          list(APPEND ComponentAbstractFactoryList "${strippedAbstractClassName}")
          list(APPEND ComponentFactoryList "${strippedClassName}")
          # There are two lists : the first contains the concrete class and the
          # second contains its abstract parent class. Then we iterate on the list to add
          # the childs to the parent correctly in the generated code.
      endforeach()

      # If there are matching strings, we add the file to our include list
      list(LENGTH fileContent matchingLines)
      if(${matchingLines} GREATER 0)
          list(APPEND ComponentFactoryFileList "${header}")
      endif()

  endforeach()

  set_property(GLOBAL PROPERTY ISCORE_${TargetName}_AbstractFactoryList ${ComponentAbstractFactoryList})
  set_property(GLOBAL PROPERTY ISCORE_${TargetName}_ConcreteFactoryList ${ComponentFactoryList})
  set_property(GLOBAL PROPERTY ISCORE_${TargetName}_FactoryFileList ${ComponentFactoryFileList})
endfunction()

function(iscore_generate_plugin_file TargetName Headers Version Uuid)
    get_property(ComponentAbstractFactoryList GLOBAL PROPERTY ISCORE_${TargetName}_AbstractFactoryList)
    get_property(ComponentFactoryList GLOBAL PROPERTY ISCORE_${TargetName}_ConcreteFactoryList)
    get_property(ComponentFactoryFileList GLOBAL PROPERTY ISCORE_${TargetName}_FactoryFileList)

    set(FactoryCode)
    set(CleanedAbstractFactoryList ${ComponentAbstractFactoryList})
    list(REMOVE_DUPLICATES CleanedAbstractFactoryList)

    list(LENGTH ComponentAbstractFactoryList LengthComponents)
    math(EXPR NumComponents ${LengthComponents}-1)

    foreach(AbstractClassName ${CleanedAbstractFactoryList})
        set(CurrentCode "FW<${AbstractClassName}")
        foreach(i RANGE ${NumComponents})
            list(GET ComponentAbstractFactoryList ${i} CurrentAbstractFactory)

            if(${AbstractClassName} STREQUAL ${CurrentAbstractFactory})
                list(GET ComponentFactoryList ${i} CurrentFactory)
                set(CurrentCode "${CurrentCode},\n    ${CurrentFactory}")
            endif()
        endforeach()
        set(CurrentCode "${CurrentCode}>\n")
        set(FactoryCode "${FactoryCode}${CurrentCode}")
    endforeach()

    # Generate a file with the list of includes
    set(FactoryFiles)
    foreach(header ${ComponentFactoryFileList})
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" strippedheader ${header})
        set(FactoryFiles "${FactoryFiles}#include <${strippedheader}>\n")
    endforeach()

    # Set the variables that will be replaces
    set(PLUGIN_NAME ${TargetName})
    set(ISCORE_COMPONENT_Version ${Version})
    set(ISCORE_COMPONENT_Uuid ${Uuid})
    configure_file(
        "${ISCORE_ROOT_SOURCE_DIR}/CMake/Components/iscore_component_plugin.hpp.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${TargetName}_plugin.hpp")
    configure_file(
        "${ISCORE_ROOT_SOURCE_DIR}/CMake/Components/iscore_component_plugin.cpp.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${TargetName}_plugin.cpp")
endfunction()


### Generate files of commands ###
function(iscore_write_file FileName Content)
    if(EXISTS "${FileName}")
      file(READ "${FileName}" EXISTING_CONTENT)
      if(NOT "${Content}" STREQUAL "${EXISTING_CONTENT}")
        file(WRITE "${FileName}" ${Content})
      endif()
    else()
        file(WRITE "${FileName}" ${Content})
    endif()
endfunction()

function(iscore_generate_command_list_file TheTarget Headers)
    # Initialize our lists
    set(commandNameList)
    set(commandFileList)

    # First look for the ISCORE_COMMAND_DECL(...) ones
    foreach(sourceFile ${Headers})
        file(READ "${sourceFile}" fileContent)
        string(REGEX MATCHALL "ISCORE_COMMAND_DECL\\([A-Za-z_0-9\,\:<>\r\n\t ]*\\(\\)[A-Za-z_0-9\,\"'\:<>\r\n\t ]*\\)"
               defaultCommands "${fileContent}")

        foreach(fileLine ${defaultCommands})
            string(REPLACE "\n" "" fileLine "${fileLine}")
            string(REPLACE "\r" "" fileLine "${fileLine}")
            string(STRIP ${fileLine} strippedLine)

            string(REPLACE "," ";" lineAsList ${strippedLine})
            list(GET lineAsList 1 commandName)
            string(STRIP ${commandName} strippedCommandName)
            list(APPEND commandNameList "${strippedCommandName}")
        endforeach()

        # If there are matching strings, we add the file to our include list
        list(LENGTH defaultCommands matchingLines)
        if(${matchingLines} GREATER 0)
            list(APPEND commandFileList "${sourceFile}")
        endif()


        # Then look for the ISCORE_COMMAND_DECL_T(...) ones
        string(REGEX MATCHALL "ISCORE_COMMAND_DECL_T\\([A-Za-z_0-9\,\:<>\r\n\t ]*\\)"
               templateCommands "${fileContent}")
        foreach(fileLine ${templateCommands})
            string(REPLACE "\n" "" fileLine "${fileLine}")
            string(REPLACE "\r" "" fileLine "${fileLine}")
            string(STRIP ${fileLine} strippedLine)

            string(REPLACE "ISCORE_COMMAND_DECL_T(" "" filtered1 ${strippedLine})
            string(REPLACE ")" "" commandName ${filtered1})
            string(STRIP ${commandName} strippedCommandName)

            list(APPEND commandNameList "${strippedCommandName}")
        endforeach()

        list(LENGTH templateCommands matchingLines)
        if(${matchingLines} GREATER 0)
            list(APPEND commandFileList "${sourceFile}")
        endif()

    endforeach()

    # Generate a file with the list of includes
    set(finalCommandFileList)
    foreach(sourceFile ${commandFileList})
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" strippedSourceFile ${sourceFile})
        set(finalCommandFileList "${finalCommandFileList}#include <${strippedSourceFile}>\n")
    endforeach()

    iscore_write_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${TheTarget}_commands_files.hpp"
        "${finalCommandFileList}"
        )

    # Generate a file with the list of types
    string(REPLACE ";" ", \n" commaSeparatedCommandList "${commandNameList}")
    iscore_write_file(
         "${CMAKE_CURRENT_BINARY_DIR}/${TheTarget}_commands.hpp"
         "${commaSeparatedCommandList}"
        )

endfunction()

function(iscore_write_static_plugins_header)
  # TODO use iscore_write_file here
  set(ISCORE_PLUGINS_FILE "${ISCORE_ROOT_BINARY_DIR}/iscore_static_plugins.hpp")
  file(WRITE "${ISCORE_PLUGINS_FILE}" "#pragma once\n#include <QtPlugin>\n")
  foreach(plugin ${ISCORE_PLUGINS_LIST})
    message("Linking statically with i-score plugin : ${plugin}")
    file(APPEND "${ISCORE_PLUGINS_FILE}" "Q_IMPORT_PLUGIN(${plugin})\n")
  endforeach()
endfunction()

### Adds tests ###
function(setup_iscore_tests TestFolder)
  if(NOT DEPLOYMENT_BUILD AND NOT ISCORE_STATIC_QT AND NOT IOS)
    add_subdirectory(${TestFolder})
  endif()
endfunction()
