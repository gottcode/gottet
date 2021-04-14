/***********************************************************************
 *
 * Copyright (C) 2008-2021 Graeme Gott <graeme@gottcode.org>
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

#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
class QAction;
class QLabel;
class Board;

class Window : public QMainWindow
{
	Q_OBJECT

public:
	Window(QWidget *parent = 0);

protected:
	virtual void closeEvent(QCloseEvent* event);

private slots:
	void pauseAvailable(bool available);
	void togglePaused(bool paused);
	void scoreUpdated(int score);
	void showScores();
	void newGame();
	void gameOver(int level, int lines, int score);
	void about();
	void setLocale();

private:
	Board* m_board;
	QAction* m_pause_action;
	QLabel* m_preview;
	QLabel* m_level;
	QLabel* m_lines;
	QLabel* m_score;
};

#endif // WINDOW_H
