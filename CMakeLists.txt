##
# Prototype desktop widget similar to kde plasmoid. Remembers
# it's position and size on screen. Hovered Pin Button
# allows click to show or hide the widgets movement & sizing
# frame.
#
# Widget views are transparent (see-through to desktop) &
# transparent to clicks. Desktop icons for example can be
# clicked "through" the Widget draw canvas.
#
# Widget draw canvas can be customized to new views easily.
#
set(APP_NAME "StorminNormin")
set(APP_DESC "A falling drifting stormy field that")
set(APP_DESC "${APP_DESC} mesmerizes on your desktop.")

set(APP_VERSION "26.09.30")
set(APP_VERSION_MINOR "00")

set(APP_AUTHOR "Mark James Capella")
set(ORG_NAME "skyrmion.inc")
set(SOURCE_REPO "https://github.com/markcapella/${APP_NAME}")

# Critical Qt build flags.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Define escape codes for error messages.
string(ASCII 27 ESC)
set(COLOR_RED    "${ESC}[31m")
set(COLOR_GREEN  "${ESC}[32m")
set(COLOR_YELLOW "${ESC}[33m")
set(COLOR_NORMAL "${ESC}[m")

cmake_minimum_required(VERSION 3.16)

project(${APP_NAME} VERSION ${APP_VERSION} LANGUAGES CXX)
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)

# Enable full threading.
if (NOT DEFINED ENV{CMAKE_BUILD_PARALLEL_LEVEL})
    message(STATUS "${COLOR_GREEN}All cores enabled.${COLOR_NORMAL}")
    cmake_host_system_information(RESULT num_cores
        QUERY NUMBER_OF_LOGICAL_CORES)
    set(ENV{CMAKE_BUILD_PARALLEL_LEVEL} "${num_cores}")
endif()

# Enable ccache.
find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}"
        CACHE STRING "C compiler launcher")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}"
        CACHE STRING "C++ compiler launcher")
    message(STATUS "${COLOR_GREEN}ccache enabled.${COLOR_NORMAL}")
else()
    message(STATUS "${COLOR_YELLOW}ccache not enabled.${COLOR_NORMAL}")
endif()

##
# Dependency Checking & Auto-APT Message Generator.
#
find_package(PkgConfig REQUIRED QUIET)

# Detect Package Manager
find_program(DNF_CMD dnf)
find_program(APT_CMD apt-get)

if (DNF_CMD)
    set(PKG_INDEX "3")
    set(PKG_MGR_INSTALL "To fix, run:\n\n   sudo dnf install")
elseif (APT_CMD)
    set(PKG_INDEX "2")
    set(PKG_MGR_INSTALL "To fix, run:\n\n   sudo apt install")
else()
    set(PKG_INDEX "2")
    set(PKG_MGR_INSTALL "To fix, install with your package manager:\n\n   ")
endif()

set(MISSING_PACKAGES "")
set(DEPENDENCY_LIST
    "PROG|git|git|git"
    "PROG|make|build-essential|@development-tools"
    "PROG|wmctrl|wmctrl|wmctrl"
    "PROG|glslc|glslc|shaderc-tools"
    "CMAKE|Qt6Core|qt6-base-dev|qt6-qtbase-devel"
    "CMAKE|Qt6Widgets|qt6-base-dev|qt6-qtbase-devel"
    "CMAKE|TBB|libtbb-dev|tbb-devel"
    "CMAKE|PNG|libpng-dev|libpng-devel"
    "CMAKE|Vulkan|libvulkan-dev|vulkan-headers"
    "CMAKE|X11|libx11-dev|libX11-devel"
    "PKG|xft|libxft-dev|libXft-devel"
    "PKG|xfixes|libxfixes-dev|libXfixes-devel"
    "PKG|xrender|libxrender-dev|libXrender-devel"
    "PKG|xext|libxext-dev|libXext-devel"
    "PKG|xcomposite|libxcomposite-dev|libXcomposite-devel"
    "PKG|glib-2.0|libglib2.0-dev|glib2-devel"
    "PKG|dbus-1|libdbus-1-dev|dbus-devel"
    "PKG|fontconfig|libfontconfig1-dev|fontconfig-devel"
    "PKG|freetype2|libfreetype6-dev|freetype-devel"
    "PKG|xkbcommon|libxkbcommon-dev|libxkbcommon-devel"
    "PKG|xcomposite|libxcomposite-dev|libXcomposite-devel"
)

foreach(ITEM IN LISTS DEPENDENCY_LIST)
    string(REPLACE "|" ";" ITEM_SPLIT "${ITEM}")
    list(GET ITEM_SPLIT 0 FIND_TYPE)
    list(GET ITEM_SPLIT 1 TARGET_NAME)
    list(GET ITEM_SPLIT ${PKG_INDEX} DISTRO_PKG)

    set(FOUND_FLAG FALSE)
    if (FIND_TYPE STREQUAL "PROG")
        find_program(PROG_PATH_${TARGET_NAME} ${TARGET_NAME} QUIET)
        if (PROG_PATH_${TARGET_NAME})
            set(FOUND_FLAG TRUE)
        endif()
    elseif (FIND_TYPE STREQUAL "CMAKE")
        find_package(${TARGET_NAME} QUIET)
        if (${TARGET_NAME}_FOUND OR ${${TARGET_NAME}_FOUND})
            set(FOUND_FLAG TRUE)
        endif()
    elseif (FIND_TYPE STREQUAL "PKG")
        pkg_check_modules(PKG_${TARGET_NAME} QUIET ${TARGET_NAME})
        if (PKG_${TARGET_NAME}_FOUND)
            set(FOUND_FLAG TRUE)
        endif()
    endif()

    if (NOT FOUND_FLAG)
        list(APPEND MISSING_PACKAGES ${DISTRO_PKG})
    endif()
endforeach()

# Warn & stop with missing packages list.
if (MISSING_PACKAGES)
    list(REMOVE_DUPLICATES MISSING_PACKAGES)
    string(JOIN " " MISSING_PKG_STR ${MISSING_PACKAGES})
    set(STATUS0 "${COLOR_YELLOW}\nRequired build dependencies are missing.")
    message(NOTICE "${STATUS0}")
    set(STATUS1 "${COLOR_GREEN}${PKG_MGR_INSTALL} ${MISSING_PKG_STR}")
    set(STATUS1 "${STATUS1}${COLOR_NORMAL}")
    message(NOTICE "")
    message(NOTICE "${STATUS1}")
    message(NOTICE "")
    message(FATAL_ERROR "")
endif()

##
# Build widget project.
#
set(WIDGET_PROJECT ${APP_NAME})

##
# Build Vulcan graphics shaders.
#
set(SHADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/flakeShader.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/flakeShader.frag
)

set(SPV_SHADERS "")
foreach(SHADER ${SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(SPV_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/shaders/${SHADER_NAME}.spv")
    # Run glslc whenever the source shader file changes
    add_custom_command(
        OUTPUT ${SPV_OUTPUT}
        DEPENDS ${SHADER}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/shaders"
        COMMAND Vulkan::glslc "${SHADER}" -o "${SPV_OUTPUT}"
        COMMENT "Compiling ${SHADER_NAME} to SPIR-V"
    )
    list(APPEND SPV_SHADERS ${SPV_OUTPUT})
endforeach()

# Create a custom target so these compile alongside your app
add_custom_target(CompileShaders DEPENDS ${SPV_SHADERS})

# All input sources.
set(WIDGET_SOURCES
    Canvas.cpp
    PinButton.cpp QuitButton.cpp MoveButton.cpp SizeButton.cpp
    ConfigDialog.cpp ColorButton.cpp AboutDialog.cpp ConfigButton.cpp
    SettingsHelper.cpp RecentsHelper.cpp DisplayHelper.cpp XHelper.cpp
    TranslationHelper.cpp
    StickyWindow.cpp StickyWidgetIII.cpp
)

# Final executable.
add_executable(${WIDGET_PROJECT} ${WIDGET_SOURCES})

# Make sure shaders compile before the app builds.
add_dependencies(${WIDGET_PROJECT} CompileShaders)

# Define app pngs.
set(ICON_PATH "share/icons/hicolor/scalable/apps")

# Create C++ Global #defines.
target_compile_options(${WIDGET_PROJECT} PRIVATE -pthread)

target_compile_definitions(${WIDGET_PROJECT} PRIVATE
    SHADER_DIR="${CMAKE_INSTALL_PREFIX}/share/${APP_NAME}/shaders/"
    APP_NAME="${APP_NAME}"
    APP_DESC="${APP_DESC}"
    APP_VERSION="${APP_VERSION}"
    APP_VERSION_MINOR="${APP_VERSION_MINOR}"
    APP_AUTHOR="${APP_AUTHOR}"
    ORG_NAME="${ORG_NAME}"
    SOURCE_REPO="${SOURCE_REPO}"
    ICON_PATH="${CMAKE_INSTALL_PREFIX}/${ICON_PATH}/"
    USE_TBB
)

# All Include sources.
target_include_directories(${WIDGET_PROJECT} PRIVATE
    ${X11_INCLUDE_DIR}
    ${PKG_xcomposite_INCLUDE_DIRS}
    ${XFIXES_INCLUDE_DIRS}
    ${XRENDER_INCLUDE_DIRS}
    ${XEXT_INCLUDE_DIRS}
    ${XCOMP_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
)

# All linker libs.
target_link_options(${WIDGET_PROJECT} PRIVATE -pthread)

target_link_libraries(${WIDGET_PROJECT} PRIVATE
    Qt6::Core Qt6::Widgets
    ${X11_LIBRARIES}
    ${PKG_xcomposite_LIBRARIES}
    X11::Xft
    ${XFIXES_LIBRARIES}
    ${XRENDER_LIBRARIES}
    ${XEXT_LIBRARIES}
    ${XCOMP_LIBRARIES}
    ${PNG_LIBRARIES}
    TBB::tbb
    Vulkan::Vulkan
)

##
# Display leak report after app runs.
#
# Debug #1.
#
# target_compile_options(${WIDGET_PROJECT} PRIVATE
#     -g -O1 -Wall -Wextra -Wuninitialized -Wmaybe-uninitialized)
#
# Debug #2.
#
# target_compile_options(${WIDGET_PROJECT} PUBLIC
#     -g -O1 -fsanitize=address -fno-omit-frame-pointer)
# target_link_options(${WIDGET_PROJECT} PUBLIC
#     -fsanitize=address)

##
# Halt any old active instances.
#
install(CODE "set(APP_NAME \"${APP_NAME}\")")
install(SCRIPT haltObsolete.cmake)

##
# Install executable.
#
install(TARGETS ${WIDGET_PROJECT} DESTINATION bin)

##
# Install App & Button icons.
#
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}.png"
    DESTINATION "${ICON_PATH}"
        RENAME ${APP_NAME}.png)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}-pinIn.png"
    DESTINATION "${ICON_PATH}"
        RENAME ${APP_NAME}-pinIn.png)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}-pinOut.png"
    DESTINATION "${ICON_PATH}"
        RENAME ${APP_NAME}-pinOut.png)

##
# Install .desktop.
#
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/desktop.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}.desktop"
    @ONLY
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}.desktop"
    DESTINATION share/applications)

##
# Install compiled SPIR-V shaders to the system path.
#
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders/ 
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${APP_NAME}/shaders)

##
# Reset Lxqt panel, & FreeDesktop and XDG cache updates
# so the panel sees the changes immediately.
#
install(CODE "
    message(STATUS \"Updating icon and desktop caches...\")
    execute_process(COMMAND update-desktop-database)
    execute_process(COMMAND xdg-icon-resource forceupdate --theme hicolor)")

##
# Setup convenience target |make uninstall|.
#
if (NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY
    )

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND}
            -DAPP_NAME=${APP_NAME}
            -P "${CMAKE_CURRENT_SOURCE_DIR}/haltObsolete.cmake"

        COMMAND ${CMAKE_COMMAND}
            -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    )
endif()
