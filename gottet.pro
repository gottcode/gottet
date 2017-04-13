lessThan(QT_VERSION, 5.2) {
	error("Gottet requires Qt 5.2 or greater")
}

TEMPLATE = app
QT += widgets
CONFIG += warn_on c++11

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x050800
DEFINES += QT_NO_NARROWING_CONVERSIONS_IN_CONNECT

# Allow in-tree builds
!win32 {
	MOC_DIR = build
	OBJECTS_DIR = build
	RCC_DIR = build
}

# Set program version
VERSION = 1.1.3
DEFINES += VERSIONSTR=\\\"$${VERSION}\\\"

# Set program name
unix: !macx {
	TARGET = gottet
} else {
	TARGET = Gottet
}

# Specify program sources
HEADERS = src/board.h \
	src/locale_dialog.h \
	src/piece.h \
	src/score_board.h \
	src/window.h

SOURCES = src/board.cpp \
	src/locale_dialog.cpp \
	src/main.cpp \
	src/piece.cpp \
	src/score_board.cpp \
	src/window.cpp

# Generate translations
TRANSLATIONS = $$files(translations/gottet_*.ts)
qtPrepareTool(LRELEASE, lrelease)
updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$LRELEASE -silent ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
updateqm.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += updateqm

# Install program data
macx {
	ICON = icons/gottet.icns
} else:win32 {
	RC_FILE = icons/icon.rc
} else:unix {
	RESOURCES = icons/icon.qrc

	isEmpty(PREFIX) {
		PREFIX = /usr/local
	}
	isEmpty(BINDIR) {
		BINDIR = bin
	}

	target.path = $$PREFIX/$$BINDIR/

	icon.files = icons/hicolor/*
	icon.path = $$PREFIX/share/icons/hicolor/

	pixmap.files = icons/gottet.xpm
	pixmap.path = $$PREFIX/share/pixmaps/

	desktop.files = icons/gottet.desktop
	desktop.path = $$PREFIX/share/applications/

	appdata.files = icons/gottet.appdata.xml
	appdata.path = $$PREFIX/share/metainfo/

	qm.files = translations/*.qm
	qm.path = $$PREFIX/share/gottet/translations
	qm.CONFIG += no_check_exist

	man.files = doc/gottet.6
	man.path = $$PREFIX/share/man/man6

	INSTALLS += target icon pixmap desktop appdata qm man
}
