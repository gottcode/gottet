# SPDX-FileCopyrightText: 2021-2025 Graeme Gott <graeme@gottcode.org>
#
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.16)

# Configure project
project(gottet VERSION 1.2.6 LANGUAGES CXX)

set(project_copyright "2008-2025 Graeme Gott")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui LinguistTools Widgets)
include(GNUInstallDirs)

add_compile_definitions(
	QT_NO_KEYWORDS
	$<$<CONFIG:DEBUG>:QT_STRICT_ITERATORS>
	$<$<CONFIG:DEBUG>:QT_NO_NARROWING_CONVERSIONS_IN_CONNECT>
	$<$<CONFIG:DEBUG>:QT_DISABLE_DEPRECATED_BEFORE=0x060800>
)

# Version number
include(cmake/AddVersionCompileDefinition.cmake)
add_version_compile_definition(src/main.cpp VERSIONSTR)

# Create program
qt_add_executable(gottet
	# Headers
	src/board.h
	src/locale_dialog.h
	src/piece.h
	src/scores_dialog.h
	src/window.h
	# Sources
	src/board.cpp
	src/locale_dialog.cpp
	src/main.cpp
	src/piece.cpp
	src/scores_dialog.cpp
	src/window.cpp
	# Resources
	${translations_QM}
)

target_link_libraries(gottet PRIVATE
	Qt6::Core
	Qt6::Gui
	Qt6::Widgets
)

# Create translations
file(GLOB translations_SRCS translations/*.ts)
qt_add_translations(gottet
	TS_FILES ${translations_SRCS}
	QM_FILES_OUTPUT_VARIABLE translations_QM
	LUPDATE_OPTIONS -no-obsolete -locations none
)

# Optimize build
option(ENABLE_LINK_TIME_OPTIMIZATION "Enable link time optimization" OFF)
if(ENABLE_LINK_TIME_OPTIMIZATION)
	include(CheckIPOSupported)
	check_ipo_supported(RESULT result)
	if(result)
		set_target_properties(gottet PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
	endif()
endif()

option(ENABLE_STRIP "Enable automatic stripping of builds" OFF)
if(ENABLE_STRIP)
	add_custom_command(TARGET gottet
		POST_BUILD
		COMMAND ${CMAKE_STRIP} $<TARGET_FILE:gottet>
	)
endif()

# Install
if(APPLE)
	set(datadir "../Resources")

	set_target_properties(gottet PROPERTIES
		OUTPUT_NAME Gottet
		MACOSX_BUNDLE TRUE
		MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/mac/Info.plist.in
	)

	include(cmake/BundleResources.cmake)
	bundle_data(gottet ${CMAKE_SOURCE_DIR}/icons/gottet.icns Resources)
	bundle_translations(gottet "${translations_QM}")
elseif(WIN32)
	set(datadir ".")

	# Use Qt6 macro until CMake provides something
	# https://bugreports.qt.io/browse/QTBUG-87618
	set_target_properties(gottet PROPERTIES
		OUTPUT_NAME Gottet
		WIN32_EXECUTABLE TRUE
		QT_TARGET_VERSION "${PROJECT_VERSION}"
		QT_TARGET_COMPANY_NAME "Graeme Gott"
		QT_TARGET_DESCRIPTION "Falling blocks game"
		QT_TARGET_COPYRIGHT "\\xA9 ${project_copyright}"
		QT_TARGET_PRODUCT_NAME "Gottet"
		QT_TARGET_RC_ICONS ${CMAKE_SOURCE_DIR}/icons/gottet.ico
	)
	_qt_internal_generate_win32_rc_file(gottet)
else()
	file(RELATIVE_PATH datadir ${CMAKE_INSTALL_FULL_BINDIR} ${CMAKE_INSTALL_FULL_DATADIR}/gottet)

	target_sources(gottet PRIVATE icons/icon.qrc)

	install(TARGETS gottet RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
	install(FILES ${translations_QM} DESTINATION ${CMAKE_INSTALL_DATADIR}/gottet/translations)
	install(FILES doc/gottet.6 DESTINATION ${CMAKE_INSTALL_MANDIR}/man6 COMPONENT doc)
	install(DIRECTORY icons/hicolor DESTINATION ${CMAKE_INSTALL_DATADIR}/icons)

	include(cmake/TranslateMetainfo.cmake)
	process_and_install_metainfo(PO_DIR ${CMAKE_SOURCE_DIR}/icons/po)
endif()

set_property(SOURCE src/main.cpp APPEND PROPERTY COMPILE_DEFINITIONS GOTTET_DATADIR="${datadir}")
