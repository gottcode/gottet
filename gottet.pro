TEMPLATE = app
CONFIG += warn_on release
macx {
	# Uncomment the following line to compile on PowerPC Macs
	# QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.4u.sdk
	CONFIG += x86 ppc
}

MOC_DIR = build
OBJECTS_DIR = build
RCC_DIR = build

unix: !macx {
	TARGET = gottet
} else {
	TARGET = Gottet
}

HEADERS = src/board.h \
	src/piece.h \
	src/score_board.h \
	src/window.h

SOURCES = src/board.cpp \
	src/main.cpp \
	src/piece.cpp \
	src/score_board.cpp \
	src/window.cpp

macx {
	ICON = gottet.icns
} else:unix {
	RESOURCES = icon.qrc
} else:win32 {
	RC_FILE = icon.rc
}

unix: !macx {
	isEmpty(PREFIX) {
		PREFIX = /usr/local
	}

	binary.path = $$PREFIX/bin/
	binary.files = gottet

	icon.path = $$PREFIX/share/icons/hicolor/48x48/apps
	icon.files = gottet.png

	desktop.path = $$PREFIX/share/applications/
	desktop.files = gottet.desktop

	INSTALLS += binary icon desktop
}
