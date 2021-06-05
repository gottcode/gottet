/*
	SPDX-FileCopyrightText: 2007-2021 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "locale_dialog.h"
#include "scores_dialog.h"
#include "window.h"

#include <QApplication>
#include <QCommandLineParser>

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

	LocaleDialog::loadTranslator("gottet_");

	ScoresDialog::migrate();

	QCommandLineParser parser;
	parser.setApplicationDescription(Window::tr("A simple falling blocks game"));
	parser.addHelpOption();
	parser.addVersionOption();
	parser.process(app);

	Window window;
	window.show();

	return app.exec();
}
