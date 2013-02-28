/***********************************************************************
 *
 * Copyright (C) 2007, 2008, 2012, 2013 Graeme Gott <graeme@gottcode.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "locale_dialog.h"
#include "window.h"

#include <QApplication>

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	app.setApplicationName("Gottet");
	app.setApplicationVersion(VERSIONSTR);
	app.setOrganizationDomain("gottcode.org");
	app.setOrganizationName("GottCode");
	{
		QIcon fallback(":/hicolor/256x256/apps/gottet.png");
		fallback.addFile(":/hicolor/128x128/apps/gottet.png");
		fallback.addFile(":/hicolor/64x64/apps/gottet.png");
		fallback.addFile(":/hicolor/48x48/apps/gottet.png");
		fallback.addFile(":/hicolor/32x32/apps/gottet.png");
		fallback.addFile(":/hicolor/24x24/apps/gottet.png");
		fallback.addFile(":/hicolor/22x22/apps/gottet.png");
		fallback.addFile(":/hicolor/16x16/apps/gottet.png");
#if (QT_VERSION >= (QT_VERSION_CHECK(4,6,0)))
		app.setWindowIcon(QIcon::fromTheme("gottet", fallback));
#else
		app.setWindowIcon(fallback);
#endif
	}

	LocaleDialog::loadTranslator("gottet_");

	Window window;
	window.show();

	return app.exec();
}
