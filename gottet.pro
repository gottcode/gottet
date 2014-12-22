lessThan(QT_VERSION, 5.2) {
	error("Gottet requires Qt 5.2 or greater")
}

TEMPLATE = app
QT += widgets
CONFIG += warn_on c++11

VERSION = 1.0.6
DEFINES += VERSIONSTR=\\\"$${VERSION}\\\"

unix: !macx {
	TARGET = gottet
} else {
	TARGET = Gottet
}

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

TRANSLATIONS = $$files(translations/gottet_*.ts)

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
	appdata.path = $$PREFIX/share/appdata/

	qm.files = translations/*.qm
	qm.path = $$PREFIX/share/gottet/translations

	man.files = doc/gottet.6
	man.path = $$PREFIX/share/man/man6

	INSTALLS += target icon pixmap desktop appdata qm man
}
