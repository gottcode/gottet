/*
	SPDX-FileCopyrightText: 2007-2022 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "locale_dialog.h"
#include "scores_dialog.h"
#include "window.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QSettings>

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	app.setApplicationName("Gottet");
	app.setApplicationVersion(VERSIONSTR);
	app.setApplicationDisplayName(Window::tr("Gottet"));
	app.setOrganizationDomain("gottcode.org");
	app.setOrganizationName("GottCode");
#if !defined(Q_OS_WIN) && !defined(Q_OS_MAC)
	app.setWindowIcon(QIcon::fromTheme("gottet", QIcon(":/gottet.png")));
	app.setDesktopFileName("gottet");
#endif

	// Find application data
	const QString appdir = app.applicationDirPath();
	const QStringList datadirs{
#if defined(Q_OS_MAC)
		appdir + "/../Resources"
#elif defined(Q_OS_UNIX)
		DATADIR,
		appdir + "/../share/gottet"
#else
		appdir
#endif
	};

	// Handle portability
#ifdef Q_OS_MAC
	const QFileInfo portable(appdir + "/../../../Data");
#else
	const QFileInfo portable(appdir + "/Data");
#endif
	if (portable.exists() && portable.isWritable()) {
		QSettings::setDefaultFormat(QSettings::IniFormat);
		QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, portable.absoluteFilePath() + "/Settings");
	}

	// Load application language
	LocaleDialog::loadTranslator("gottet_", datadirs);

	// Handle commandline
	QCommandLineParser parser;
	parser.setApplicationDescription(Window::tr("A simple falling blocks game"));
	parser.addHelpOption();
	parser.addVersionOption();
	parser.process(app);

	// Convert old scores to new format
	ScoresDialog::migrate();

	// Create main window
	Window window;
	window.show();

	return app.exec();
}
