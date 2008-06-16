TEMPLATE = app
CONFIG += warn_on release
macx {
	# Uncomment the following line to compile on PowerPC Macs
	# QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.4u.sdk
	CONFIG += x86 ppc
}

qws {
	qtopia_project(qtopia app)
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
	ICON = icons/gottet.icns
} else:unix {
	RESOURCES = icons/icon.qrc
} else:win32 {
	RC_FILE = icons/icon.rc
}

unix: !macx {
	isEmpty(PREFIX) {
		PREFIX = /usr/local
	}

	binary.files = gottet
	icon.files = icons/gottet.png
	desktop.files = icons/gottet.desktop

	qws {
		binary.path = /bin/
		icon.path = /pics/gottet
		icon.hint = pics
		desktop.path = /apps/Games
		desktop.hint = desktop
	} else {
		binary.path = $$PREFIX/bin/
		icon.path = $$PREFIX/share/icons/hicolor/48x48/apps
		desktop.path = $$PREFIX/share/applications/
	}

	INSTALLS += binary icon desktop
}
