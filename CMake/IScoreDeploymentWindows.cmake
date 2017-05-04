if(NOT WIN32)
return()
endif()

set(ISCORE_BIN_INSTALL_DIR "bin")

### TODO InstallRequiredSystemLibraries ###
 # Compiler Runtime DLLs
if (MSVC)
   # Visual Studio
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP true)
    include(InstallRequiredSystemLibraries)
    install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
            DESTINATION ${ISCORE_BIN_INSTALL_DIR})
else()
   # MinGW
    get_filename_component(MINGW_DLL_DIR ${CMAKE_CXX_COMPILER} PATH)
    install(FILES
            "${MINGW_DLL_DIR}/libgcc_s_dw2-1.dll"
            "${MINGW_DLL_DIR}/libstdc++-6.dll"
            "${MINGW_DLL_DIR}/libwinpthread-1.dll"
            DESTINATION ${ISCORE_BIN_INSTALL_DIR})
endif()


# Qt Libraries
set(DEBUG_CHAR "$<$<CONFIG:Debug>:d>")

get_target_property(QtCore_LOCATION Qt5::Core LOCATION)
get_filename_component(QT_DLL_DIR ${QtCore_LOCATION} PATH)
file(GLOB ICU_DLLS "${QT_DLL_DIR}/icu*.dll")

# TODO instead register them somewhere like the plug-ins.



if(NOT OSSIA_STATIC)
install(FILES "$<TARGET_FILE:ossia>"
        DESTINATION ${ISCORE_BIN_INSTALL_DIR})
endif()

install(FILES
  ${ICU_DLLS}
  "${QT_DLL_DIR}/Qt5Core${DEBUG_CHAR}.dll"
  "${QT_DLL_DIR}/Qt5Gui${DEBUG_CHAR}.dll"
  "${QT_DLL_DIR}/Qt5Widgets${DEBUG_CHAR}.dll"
  "${QT_DLL_DIR}/Qt5Network${DEBUG_CHAR}.dll"
  "${QT_DLL_DIR}/Qt5Xml${DEBUG_CHAR}.dll"
  "${QT_DLL_DIR}/Qt5Svg${DEBUG_CHAR}.dll"
  "${QT_DLL_DIR}/Qt5Qml${DEBUG_CHAR}.dll"
  "${QT_DLL_DIR}/Qt5OpenGL${DEBUG_CHAR}.dll"
  "${QT_DLL_DIR}/Qt5WebSockets${DEBUG_CHAR}.dll"
#  "${QT_DLL_DIR}/Qt5Test${DEBUG_CHAR}.dll"
  "${QT_DLL_DIR}/Qt5Quick${DEBUG_CHAR}.dll"
  "${QT_DLL_DIR}/Qt5QuickWidgets${DEBUG_CHAR}.dll"
  "${QT_DLL_DIR}/Qt5QuickControls2${DEBUG_CHAR}.dll"
  "${QT_DLL_DIR}/Qt5QuickTemplates2${DEBUG_CHAR}.dll"
  DESTINATION ${ISCORE_BIN_INSTALL_DIR})

# Qt conf file
install(
  FILES
    "${ISCORE_ROOT_SOURCE_DIR}/CMake/Deployment/Windows/qt.conf"
    "${ISCORE_ROOT_SOURCE_DIR}/base/lib/resources/i-score.ico"
  DESTINATION
    ${ISCORE_BIN_INSTALL_DIR})

# Qt plug-ins
set(QT_PLUGINS_DIR "${QT_DLL_DIR}/../plugins")
set(QT_QML_PLUGINS_DIR "${QT_DLL_DIR}/../qml")
set(plugin_dest_dir "${ISCORE_BIN_INSTALL_DIR}/plugins")

install(FILES "${QT_PLUGINS_DIR}/platforms/qwindows${DEBUG_CHAR}.dll" DESTINATION "${plugin_dest_dir}/platforms")
install(FILES "${QT_PLUGINS_DIR}/imageformats/qsvg${DEBUG_CHAR}.dll" DESTINATION "${plugin_dest_dir}/imagesformats")
install(FILES "${QT_PLUGINS_DIR}/iconengines/qsvgicon${DEBUG_CHAR}.dll" DESTINATION "${plugin_dest_dir}/iconengines")
install(DIRECTORY "${QT_QML_PLUGINS_DIR}/QtQuick" "${QT_QML_PLUGINS_DIR}/QtQuick.2" DESTINATION "${ISCORE_BIN_INSTALL_DIR}/qml")

install(CODE "
    file(GLOB_RECURSE DLLS_TO_REMOVE \"*.dll\")
    list(FILTER DLLS_TO_REMOVE INCLUDE REGEX \"qml/.*/*dll\")
    file(REMOVE \${DLLS_TO_REMOVE})
    ")

install(FILES "${QT_QML_PLUGINS_DIR}/QtQuick.2/qtquick2plugin${DEBUG_CHAR}.dll" DESTINATION "${ISCORE_BIN_INSTALL_DIR}/qml/QtQuick.2")
# NSIS metadata
set(CPACK_GENERATOR "NSIS")
set(CPACK_PACKAGE_EXECUTABLES "i-score.exe;i-score")

set(CPACK_COMPONENTS_ALL i-score)

set(CPACK_MONOLITHIC_INSTALL TRUE)
set(CPACK_NSIS_PACKAGE_NAME "i-score")
set(CPACK_PACKAGE_ICON "${ISCORE_ROOT_SOURCE_DIR}\\\\base\\\\lib\\\\resources\\\\i-score.ico")
set(CPACK_NSIS_MUI_ICON "${CPACK_PACKAGE_ICON}")
set(CPACK_NSIS_MUI_UNIICON "${CPACK_PACKAGE_ICON}")

set(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.i-score.org")
set(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.i-score.org")
set(CPACK_NSIS_CONTACT "i-score-devs@lists.sourceforge.net")

set(CPACK_NSIS_COMPRESSOR "/SOLID lzma")

set(CPACK_NSIS_MENU_LINKS
    "bin/i-score.exe" "i-score"
    "http://www.i-score.org" "i-score website"
    )


set(CPACK_NSIS_DEFINES "!include ${CMAKE_CURRENT_LIST_DIR}\\\\Deployment\\\\Windows\\\\FileAssociation.nsh")
set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
\\\${registerExtension} '\\\$INSTDIR\\\\bin\\\\i-score.exe' '.scorejson' 'i-score score'

SetOutPath '\\\$INSTDIR\\\\bin'
CreateShortcut '\\\$DESKTOP\\\\i-score.lnk' '\\\$INSTDIR\\\\bin\\\\i-score.exe' '' '\\\$INSTDIR\\\\bin\\\\i-score.ico'
SetRegView 64
WriteRegStr HKEY_LOCAL_MACHINE 'SOFTWARE\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\App Paths\\\\i-score.exe' '' '$INSTDIR\\\\bin\\\\i-score.exe'
WriteRegStr HKEY_LOCAL_MACHINE 'SOFTWARE\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\App Paths\\\\i-score.exe' 'Path' '$INSTDIR\\\\bin\\\\;$INSTDIR\\\\bin\\\\plugins\\\\;'
")
set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
Delete '$DESKTOP\\\\i-score.lnk'
DeleteRegKey HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\App Paths\\\\i-score.exe'
\\\${unregisterExtension} '.scorejson' 'i-score score'
")

